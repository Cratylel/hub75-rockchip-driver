#include <iostream>
#include <fstream>
#include "config.h"
#include "log.h"
#include "../include/json.hpp"

using json = nlohmann::json;

config::configuration current_configuration;

void config::from_json(const nlohmann::json &j, config::pin_mapping &p)
    {
        j.at("OE").get_to(p.OE);
        j.at("STB").get_to(p.STB);
        j.at("CLK").get_to(p.CLK);
        j.at("A").get_to(p.A);
        j.at("B").get_to(p.B);
        j.at("C").get_to(p.C);
        j.at("D").get_to(p.D);
        j.at("E").get_to(p.E);
        j.at("R1").get_to(p.R1);
        j.at("G1").get_to(p.G1);
        j.at("B1").get_to(p.B1);
        j.at("R2").get_to(p.R2);
        j.at("G2").get_to(p.G2);
        j.at("B2").get_to(p.B2);
    }

int config::init(std::string config_path)
{   
    LOG_INFO("Loading configuration file from path: %s", config_path.c_str());
    std::ifstream file(config_path);
    json config;

    LOG_DEBUG("Parsing JSON file");
    
    file >> config;
    
    LOG_VERBOSE("Reading configuration values");
    current_configuration.width = config["width"];
    current_configuration.height = config["height"];
    current_configuration.refresh_rate = config["refresh_rate"];
    current_configuration.buffer_path = config["buffer_path"];

    LOG_VERBOSE("Reading pin mappings");
    std::string selected = config["selected_mapping"];
    config::pin_mapping pins = config["mappings"][selected];
    current_configuration.pins = pins;

    return 0;
}

uint32_t config::get_width()
{
    LOG_VERBOSE("Reading width from configuration: %d", current_configuration.width);
    return current_configuration.width;
}

uint32_t config::get_height()
{
    LOG_VERBOSE("Reading height from configuration: %d", current_configuration.height);
    return current_configuration.height;
}

uint32_t config::get_refresh_rate()
{
    LOG_VERBOSE("Reading refresh rate from configuration: %d", current_configuration.refresh_rate);
    return current_configuration.refresh_rate;
}

std::string config::get_buffer_path()
{
    LOG_VERBOSE("Reading buffer path from configuration: %s", current_configuration.buffer_path.c_str());
    return current_configuration.buffer_path;
}

config::pin_mapping config::get_pins()
{
    LOG_VERBOSE("Retrieving pin mappings from configuration");
    return current_configuration.pins;
}