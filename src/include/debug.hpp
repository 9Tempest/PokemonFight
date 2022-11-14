#pragma once

#ifdef NDEBUG

// disable functions
#define DLOG(...) ((void)0)

#define INIT_LOG
#define PRINT(stmt)

#else

#include <stdio.h>

#define DLOG(fmt, ...)                                                         \
    do {                                                                       \
        fprintf(stderr, "DEBUG %lu [%s:%d] ", pthread_self(), __FILE__,         \
                __LINE__);                                                     \
        fprintf(stderr, fmt, ##__VA_ARGS__);                                   \
        fprintf(stderr, "\n");                                                 \
    } while (0)

#define PRINT(stmt) stmt
#endif // NDEBUG