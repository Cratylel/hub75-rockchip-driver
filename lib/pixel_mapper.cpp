#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include "gpio.h"
#include "pixel_mapper.h"
#include "log.h"
#include "config.h"
#include <iostream>

#include "testserver.h"

config::pin_mapping pin_map;

namespace pixel_mapper
{
    uint32_t frame_count = 0;
}


void pixel_mapper::init() {
    pixel_mapper::frame_count = 0;
}

void pixel_mapper::set_new_pin_map(config::pin_mapping new_pin_map) {
    LOG_DEBUG("Setting new pin map");
    pin_map = new_pin_map;
}

void pixel_mapper::flush_frame(pixel_mapper::frame *frame) {
    int channel_offset = frame->height*frame->width/2; //offset for channel

    GPIO::set_pin(pin_map.OE, 1); //turn off pixels

    for (uint32_t i = 0; i < frame->height/2; i++) { //cycle through rows
        int offset = i * frame->width; //offset for row
        
        // set addr to i
        GPIO::set_pin(pin_map.A, (i & 0x01) ? 1 : 0);        
        GPIO::set_pin(pin_map.B, (i & 0x02) ? 1 : 0);
        GPIO::set_pin(pin_map.C, (i & 0x04) ? 1 : 0);
        GPIO::set_pin(pin_map.D, (i & 0x08) ? 1 : 0);
        GPIO::set_pin(pin_map.E, (i & 0x10) ? 1 : 0);
        #ifdef TEST_ENV
        #ifdef TEST_SERVER
        testserver::send_pin_data("A", (i & 0x01) ? 1 : 0);
        testserver::send_pin_data("B", (i & 0x02) ? 1 : 0);
        testserver::send_pin_data("C", (i & 0x04) ? 1 : 0);
        testserver::send_pin_data("D", (i & 0x08) ? 1 : 0);
        testserver::send_pin_data("E", (i & 0x10) ? 1 : 0);
        #endif
        #endif
        // if (frame->height == 64 && pin_map.E != -1) { // if display is 64 rows and we have a pin map set E
        //     GPIO::set_pin(pin_map.E, (i & 0x10) ? 1 : 0);
        // }

        // uint32_t ii = offset; ii < frame->width+offset; ii++
        for (uint32_t ii = offset; ii < frame->width+offset; ii++) { //cycle through columns in row
            GPIO::set_pin(pin_map.R1, frame->fb[ii].R);
            GPIO::set_pin(pin_map.G1, frame->fb[ii].G);
            GPIO::set_pin(pin_map.B1, frame->fb[ii].B);
            GPIO::set_pin(pin_map.R2, frame->fb[ii+channel_offset].R);
            GPIO::set_pin(pin_map.G2, frame->fb[ii+channel_offset].G);
            GPIO::set_pin(pin_map.B2, frame->fb[ii+channel_offset].B);

            #ifdef TEST_ENV
            #ifdef TEST_SERVER
            //testserver::send_raw_data(channel_offset, offset, i, ii, frame->fb[ii].R, frame->fb[ii].G, frame->fb[ii].B, frame->fb[ii+channel_offset].R, frame->fb[ii+channel_offset].G, frame->fb[ii+channel_offset].B);
            testserver::send_pin_data("R1", frame->fb[ii].R);
            testserver::send_pin_data("G1", frame->fb[ii].G);
            testserver::send_pin_data("B1", frame->fb[ii].B);
            testserver::send_pin_data("R2", frame->fb[ii+channel_offset].R);
            testserver::send_pin_data("G2", frame->fb[ii+channel_offset].G);
            testserver::send_pin_data("B2", frame->fb[ii+channel_offset].B);
            testserver::send_pin_data("CLK", 1);
            testserver::send_pin_data("CLK", 0);
            #endif
            #endif
            
            GPIO::set_pin(pin_map.CLK, 1);
            GPIO::set_pin(pin_map.CLK, 0);
            // column is done
                  
        } // row is done

        GPIO::set_pin(pin_map.STB, 1); // latch data
        GPIO::set_pin(pin_map.STB, 0);
        #ifdef TEST_SERVER
        testserver::send_pin_data("STB", 1);
        testserver::send_pin_data("STB", 0);
        #endif
    } // frame is done

    GPIO::set_pin(pin_map.OE, 0); //turn on display

    pixel_mapper::frame_count++;
    LOG_VERBOSE("Flushed frame %d", pixel_mapper::frame_count);
}

#define OUTPUT 1

void pixel_mapper::set_output_on_pins() {

    // Set all pins on output mode

    GPIO::set_pin_mode(pin_map.R1, OUTPUT);
    GPIO::set_pin_mode(pin_map.G1, OUTPUT);
    GPIO::set_pin_mode(pin_map.B1, OUTPUT);
    GPIO::set_pin_mode(pin_map.R2, OUTPUT);
    GPIO::set_pin_mode(pin_map.G2, OUTPUT);
    GPIO::set_pin_mode(pin_map.B2, OUTPUT);
    GPIO::set_pin_mode(pin_map.OE, OUTPUT);
    GPIO::set_pin_mode(pin_map.STB, OUTPUT);
    GPIO::set_pin_mode(pin_map.CLK, OUTPUT);
    GPIO::set_pin_mode(pin_map.A, OUTPUT);
    GPIO::set_pin_mode(pin_map.B, OUTPUT);
    GPIO::set_pin_mode(pin_map.C, OUTPUT);
    GPIO::set_pin_mode(pin_map.D, OUTPUT);

    if (pin_map.E != -1) { //E is optional
        GPIO::set_pin_mode(pin_map.E, OUTPUT);
    }
    
}