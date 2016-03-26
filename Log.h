#ifndef LOG_H
#define LOG_H

#ifdef __DEBUG_LOG
#define INFOPRINT(t, ...) printf("\e[1m%s\e[0m\t", __FUNCTION__); \
                         printf(t, ##__VA_ARGS__);

#define WARNPRINT(t, ...) printf("\e[1;33m%s\e[0m\t", __FUNCTION__); \
                         printf(t, ##__VA_ARGS__);

#define FAILPRINT(t, ...) printf("\e1;31m%s\e[0m\t", __FUNCTION__); \
                         printf(t, ##__VA_ARGS__);
#else
#define INFOPRINT(t, ...)
#define WARNPRINT(t, ...)
#define FAILPRINT(t, ...)
#endif

#endif
