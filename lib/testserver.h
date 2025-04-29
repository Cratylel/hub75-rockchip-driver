#ifndef TESTSERVER_H
#define TESTSERVER_H
#include <string>

namespace testserver
{
    void init();
    void clear();
    void send_pixel_data(uint8_t x, uint8_t y, bool r, bool g, bool b);
    void send_raw_data(uint32_t channel_offset, uint32_t row_offset, uint32_t row_iteration, uint32_t col_iteration, bool r1, bool g1, bool b1, bool r2, bool g2, bool b2);

    void send_pin_data(const std::string pin_name, bool value);

} // namespace testserver



#endif