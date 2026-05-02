#include "kernel.h"
#include "esp_intr_alloc.h"
#include "xtensa/hal.h"
#include <string.h>
#include "xt_instr_macros.h"

#define CCOUNT    234
#define CCOMPARE0 240
// cfg
#define MAX_TASKS 4
#define TICKS_PER_SLICE 24000 // 1ms at 24MHz

// global for assembly extern
task_t* current_task = NULL;

static task_t* tasks[MAX_TASKS];
static int task_count = 0;
static int current_idx = 0;

#define FRAME_SIZE 72 // stack

static uint32_t* init_stack(uint32_t* stack_top, void (*entry)(void)) {
    stack_top = (uint32_t*) ((uintptr_t) stack_top & ~0xFu); // align to 16 bytes
    uint8_t* frame = (uint8_t*) stack_top - FRAME_SIZE; // simulate frame as if it was interrupted
    memset(frame, 0, FRAME_SIZE);                       // clear frame
    *(uint32_t*) (frame + 64) = (uint32_t) entry;       // EPC1 entry
    *(uint32_t*) (frame + 68) = 0x00000000;             // EPS1
    return (uint32_t*) frame;                           // new stack top
}

void task_create(task_t* t, void (*entry)(void), uint32_t* stack, size_t size) {
    t->entry = entry;
    t->stack_base = stack;
    t->stack_size = size;
    t->state = TASK_READY;
    t->sp = init_stack(stack + size / sizeof(uint32_t), entry);
    tasks[task_count++] = t;
    if (task_count == 1) current_task = tasks[0];
}

// called from isr
void scheduler_tick(void) {
    uint32_t ccount;
    RSR(CCOUNT, ccount);
    WSR(CCOMPARE0, ccount + TICKS_PER_SLICE);

    tasks[current_idx]->state = TASK_READY;
    current_idx = (current_idx + 1) % task_count;
    current_task = tasks[current_idx];
    current_task->state = TASK_RUNNING;
}