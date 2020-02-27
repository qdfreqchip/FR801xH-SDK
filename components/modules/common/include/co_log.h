#ifndef _CO_LOG_H
#define _CO_LOG_H

#include "co_printf.h"

#define LOG_ENABLE              0

#define LOG_LEVEL_NONE          0
#define LOG_LEVEL_ERROR         1
#define LOG_LEVEL_WARNING       2
#define LOG_LEVEL_INFO          3

#ifndef LOG_LEVEL_MODULE
#define LOG_LEVEL_MODULE        LOG_LEVEL_NONE
#endif

#ifndef LOG_WITH_FILE
#define LOG_WITH_FILE           0
#endif

#if LOG_ENABLE
#define LOG_INFO(...)   do {                                                    \
                            if(LOG_LEVEL_MODULE >= LOG_LEVEL_INFO) {            \
                                if(LOG_WITH_FILE) {                             \
                                    co_printf("%s %d: ", __FUNCTION__, __LINE__);   \
                                }                                               \
                                co_printf(__VA_ARGS__);                         \
                            }                                                   \
                        } while(0)

#define LOG_WARN(...)   do {                                                    \
                            if(LOG_LEVEL_MODULE >= LOG_LEVEL_WARNING) {         \
                                if(LOG_WITH_FILE) {                             \
                                    co_printf("%s %d: ", __FUNCTION__, __LINE__);   \
                                }                                               \
                                co_printf(__VA_ARGS__);                         \
                            }                                                   \
                        } while(0)

#define LOG_ERR(...)    do {                                                    \
                            if(LOG_LEVEL_MODULE >= LOG_LEVEL_ERROR) {           \
                                if(LOG_WITH_FILE) {                             \
                                    co_printf("%s %d: ", __FUNCTION__, __LINE__);   \
                                }                                               \
                                co_printf(__VA_ARGS__);                         \
                            }                                                   \
                        } while(0)

#else   // #if LOG_ENABLE
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERR(...)
#endif  // #if LOG_ENABLE

#endif  // _CO_LOG_H

