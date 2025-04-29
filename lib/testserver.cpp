#ifdef TEST_SERVER

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "testserver.h"
#include "log.h"

int sock = socket(AF_INET, SOCK_STREAM, 0);
bool test_server_active = false;

void testserver::init()
{
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(sock, (sockaddr *)&server, sizeof(server)) < 0)
    {
        test_server_active = false;
        LOG_WARN("Test server connection failed");
        return;
    }
    test_server_active = true;
    clear();
}

void testserver::clear()
{
    if (test_server_active)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "clear\n");
        send(sock, buffer, strlen(buffer), 0);
    }
}

void testserver::send_pixel_data(uint8_t x, uint8_t y, bool r, bool g, bool b)
{
    if (test_server_active)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%u,%u;%d,%d,%d\n", x, y, r, g, b);
        send(sock, buffer, strlen(buffer), 0);
    }
}

void testserver::send_raw_data(uint32_t channel_offset, uint32_t row_offset, uint32_t row_iteration, uint32_t col_iteration, bool r1, bool g1, bool b1, bool r2, bool g2, bool b2)
{
    if (test_server_active)
    {
        int x1 = 64 - (col_iteration - row_offset);
        int y1 = row_iteration;
        send_pixel_data(x1 - 1, y1, r1, g1, b1);
        send_pixel_data(x1 - 1, y1 + 16, r2, g2, b2);
    }
}

void testserver::send_pin_data(std::string pin_name, bool value)
{
    if (test_server_active)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s,%d\n", pin_name.c_str(), value);
        size_t result = send(sock, buffer, strlen(buffer), 0);
    }
}

#endif