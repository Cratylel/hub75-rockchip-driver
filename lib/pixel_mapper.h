#pragma once
#ifndef PIXEL_MAPPER_H
#define PIXEL_MAPPER_H

#include "config.h"
#include <cstdint>

namespace pixel_mapper
{   
    struct pixel {
        bool R;
        bool G;
        bool B;
    };
    struct frame
    {
        uint32_t width;
        uint32_t height;
        //pixel fb[2048]; // rows are ordered from top to bottom, but columns are ordered from right to left (hflip)
        pixel *fb;
    };
    
    void init();
    void set_new_pin_map(config::pin_mapping new_pin_map);
    void set_output_on_pins();
    void flush_frame(pixel_mapper::frame *frame);
    extern uint32_t frame_count;
} // namespace pixel_mapper



#endif