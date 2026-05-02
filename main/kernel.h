#pragma once
#include <stdint.h>
#include <stddef.h>

typedef enum {
    TASK_READY,
    TASK_RUNNING,
} task_state_t;

typedef struct {
    uint32_t *sp;
    uint32_t *stack_base;
    size_t stack_size;
    void (*entry)(void);
    task_state_t state;
} task_t;

