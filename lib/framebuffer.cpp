#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include "framebuffer.h"
#include "pixel_mapper.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "log.h"

pthread_mutex_t* mutex;
framebuffer::buffer* shared_buffer;
void* shm_ptr;

framebuffer::buffer *framebuffer::create_local_buffer(uint32_t width, uint32_t height)
{
    framebuffer::buffer *new_buffer = (framebuffer::buffer *)malloc(sizeof(framebuffer::buffer));
    if (new_buffer == NULL)
    {
        return NULL;
    }
    new_buffer->frame.width = width;
    new_buffer->frame.height = height;

    // Calculate the number of pixels and allocate memory for fb
    size_t num_pixels = width * height;
    new_buffer->frame.fb = (pixel_mapper::pixel *)malloc(sizeof(pixel_mapper::pixel) * num_pixels);
    if (new_buffer->frame.fb == NULL)
    {
        free(new_buffer);
        return NULL;
    }
    return new_buffer;
}

void framebuffer::destroy_local_buffer(framebuffer::buffer *buf)
{
    // Confirm buffer is valid before attempting to free
    if (buf != NULL)
    {
        // Ensure fb is valid before freeing
        if (buf->frame.fb != NULL)
        {
            free(buf->frame.fb); // Correctly free the allocated pixel array
            buf->frame.fb = NULL; // Avoid dangling pointer
        }
        free(buf); // Correctly free the buffer itself
    }
}

int framebuffer::map_buffer(const char *path, uint32_t width, uint32_t height){

    size_t framebuffer_size = width * height * sizeof(pixel_mapper::pixel);
    size_t total_size = sizeof(pthread_mutex_t) + sizeof(framebuffer::buffer) + framebuffer_size;
    
    LOG_INFO("Begin mapping buffer with width: %u, height: %u", width, height);

    LOG_DEBUG("Allocating %zu bytes for buffer\n", total_size);
    int shm_fd = shm_open(path, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        LOG_ERROR("Error creating shared buffer");
        return -1;
    }
    LOG_DEBUG("Resizing shared buffer to %zu bytes", total_size);
    if (ftruncate(shm_fd, total_size) == -1)
    {
        LOG_ERROR("Error resizing shared buffer");
        close(shm_fd);
        return -1;
    }

    LOG_DEBUG("Mapping shared buffer");
    shm_ptr = mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        LOG_ERROR("Error mapping shared buffer");
        close(shm_fd);
        return -1;
    }

    LOG_DEBUG("Mapping buffer to pixel array");
    mutex = (pthread_mutex_t*)shm_ptr;
    shared_buffer = (framebuffer::buffer*)((char*)shm_ptr+sizeof(pthread_mutex_t));
    pixel_mapper::pixel * fb = (pixel_mapper::pixel*)((char*)shm_ptr+sizeof(pthread_mutex_t)+sizeof(framebuffer::buffer));

    // init mutex
    LOG_DEBUG("Initializing mutex");
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    LOG_DEBUG("Initializing shared buffer");
    shared_buffer->frame.width = width;
    shared_buffer->frame.height = height;
    shared_buffer->frame.fb = fb;

    LOG_DEBUG("Shared buffer initialized");
    return 0;

}

framebuffer::buffer framebuffer::read_buffer()
{   
    framebuffer::buffer newbuffer;
    LOG_DEBUG("Reading buffer");

    if (shared_buffer == nullptr || mutex == nullptr || shm_ptr == nullptr){
        LOG_ERROR("Shared buffer memory not initialized");
        exit(1);
        return newbuffer;
    }

    LOG_VERBOSE("Locking mutex");
    
    auto start = std::chrono::high_resolution_clock::now();
    pthread_mutex_lock(mutex);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    LOG_VERBOSE("Mutex locked in %.3f milliseconds", duration.count());
    if (duration.count() > 5) {
        LOG_WARN("Took too long to lock mutex: %.3f milliseconds", duration.count());
    }

    LOG_VERBOSE("Reading shared buffer fb");
    pixel_mapper::pixel* fb = (pixel_mapper::pixel*)((char*)shm_ptr+sizeof(pthread_mutex_t)+sizeof(framebuffer::buffer));
    //pixel_mapper::frame* frame; //not used

    LOG_VERBOSE("Initializing return buffer");
    
    newbuffer.frame.height = shared_buffer->frame.height;
    newbuffer.frame.width = shared_buffer->frame.width;
    newbuffer.frame.fb = fb;

    LOG_VERBOSE("Unlocking mutex");
    pthread_mutex_unlock(mutex);

    return newbuffer;
}
