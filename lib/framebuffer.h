#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include  "pixel_mapper.h"
namespace framebuffer {

    struct buffer {
        uint32_t lock = 0;
        pixel_mapper::frame frame;
    };

    int map_buffer(const char *path, uint32_t width, uint32_t height);
    buffer read_buffer();

    buffer* create_local_buffer(uint32_t width, uint32_t height);
    void destroy_local_buffer(buffer *buf);


} // namespace framebuffer

#endif // FRAMEBUFFER_H
