#ifndef _CONFIG_H
#define _CONFIG_H
#include "../include/json.hpp"
namespace config
{
    struct pin_mapping
    {
        int OE, STB, CLK;
        int A, B, C, D, E;
        int R1, G1, B1;
        int R2, G2, B2;
    };

    void from_json(const nlohmann::json &j, pin_mapping &p);

    struct configuration {
        uint32_t width;
        uint32_t height;
        uint32_t refresh_rate;
        std::string buffer_path;
        pin_mapping pins;
    };

    int init(std::string config_path);

    uint32_t get_width();
    uint32_t get_height();
    uint32_t get_refresh_rate();
    std::string get_buffer_path();
    pin_mapping get_pins();

} // namespace config

#endif