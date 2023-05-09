#ifndef MACROS_H
#define MACROS_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define TAKE_MUTEX(mutex)                                                                                              \
    while (xSemaphoreTake(mutex, portMAX_DELAY) != pdTRUE)                                                             \
        ;

#define RELEASE_MUTEX(mutex) xSemaphoreGive(mutex);

#define TAKE_RECURSIVE_MUTEX(mutex)                                                                                    \
    while (xSemaphoreTakeRecursive(mutex, portMAX_DELAY) != pdTRUE)                                                    \
        ;

#define RELEASE_RECURSIVE_MUTEX(mutex) xSemaphoreGiveRecursive(mutex);

#endif
