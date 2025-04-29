#include "pixel_mapper.h"
#include "gpio.h"
#include "config.h"
#include <stdio.h>
#include "testserver.h"
#include "framebuffer.h"
#include <chrono>

#define CONFIG_FILE_LOCAL "./config.json"
#define CONFIG_FILE_GLOBAL "/etc/matrix/config.json"

int main()
{   
    LOG_INFO("Starting HUB75 driver");

    if (config::init(CONFIG_FILE_GLOBAL) != 0) {
        if (config::init(CONFIG_FILE_LOCAL) != 0) {
            LOG_ERROR("No config file at either %s or %s", CONFIG_FILE_GLOBAL, CONFIG_FILE_LOCAL);
            exit(1);
        } else {
            LOG_WARN("No global config file at %s, using local config file at %s instead", CONFIG_FILE_GLOBAL, CONFIG_FILE_LOCAL);
        }
    }
    LOG_INFO("Configuration loaded successfully");

    GPIO::init();
    pixel_mapper::init();
    pixel_mapper::set_output_on_pins();

#ifdef TEST_SERVER
    testserver::init();
    testserver::clear();
#endif

    pixel_mapper::set_new_pin_map(config::get_pins());

    framebuffer::map_buffer(config::get_buffer_path().c_str(), config::get_width(), config::get_height());
    framebuffer::buffer buffer = framebuffer::read_buffer();

    while (1)
    {
        // Flush the frame and measure the time taken
        buffer = framebuffer::read_buffer();

        auto start = std::chrono::high_resolution_clock::now();
        pixel_mapper::flush_frame(&buffer.frame);
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the duration and convert it to milliseconds
        std::chrono::duration<double, std::milli> duration = end - start;
        LOG_VERBOSE("Frame flush took %.3f milliseconds\n", duration.count());

        // Calculate the wait time to achieve 30 FPS
        const double targetFrameDuration = 1000.0 / config::get_refresh_rate(); // in milliseconds
        double waitTime = targetFrameDuration - duration.count();

        if (waitTime > 0)
        {
            usleep(static_cast<int>(waitTime * 1000)); // Convert milliseconds to microseconds
            LOG_VERBOSE("Waiting for %.3f milliseconds\n", waitTime);
        }
    }

    GPIO::exit();

    return 0;
}
