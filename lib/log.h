#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, "ERROR [%s:%d %s]: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) \
    fprintf(stderr, "WARN  [%s:%d %s]: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    fprintf(stdout, "INFO  [%s:%d %s]: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#ifdef LOG_LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...) \
    fprintf(stdout, "DEBUG [%s:%d %s]: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...)
#endif
#ifdef LOG_LEVEL_VERBOSE
#define LOG_VERBOSE(fmt, ...) \
    fprintf(stdout, "VERBOSE [%s:%d %s]: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOG_VERBOSE(fmt, ...)
#endif

// enum exit_codes {
//     EXIT_SUCCESS = 0,
//     EXIT_FAILURE = 1,
// };