#include "driver/uart.h"
#include "esp_intr_alloc.h"
#include "kernel.h"
#include "xt_instr_macros.h"
#include "xtensa/hal.h"
#include <stdio.h>
#include "xtensa/xtruntime.h"


#define STACK_SIZE 2048

static uint32_t stack_a[STACK_SIZE / sizeof(uint32_t)];
static uint32_t stack_b[STACK_SIZE / sizeof(uint32_t)];

static task_t task_a;
static task_t task_b;

void task_a_entry(void) {
    for (;;) {
        printf("A\n");
        for (volatile int i = 0; i < 1000000; i++)
            ;
    }
}

void task_b_entry(void) {
    for (;;) {
        printf("B\n");
        for (volatile int i = 0; i < 1000000; i++)
            ;
    }
}

// ISR
extern void _kernel_timer_isr(void);

void esp_startup_start_app(void) {
    // UART initialized by IDF

    task_create(&task_a, task_a_entry, stack_a, sizeof(stack_a));
    task_create(&task_b, task_b_entry, stack_b, sizeof(stack_b));

    // register ISR for CCOMPARE0 timer (level 1)
    xt_set_interrupt_handler(ETS_INTERNAL_TIMER0_INTR_SOURCE, (xt_handler) _kernel_timer_isr, NULL);
    xt_ints_on(1 << ETS_INTERNAL_TIMER0_INTR_SOURCE);

    uint32_t ccount;
    RSR(CCOUNT, ccount);
    WSR(CCOMPARE0, ccount + 240000);

    // jump to task_a
    __asm__ volatile("movi   a2, current_task    \n"
                     "l32i   a2, a2, 0           \n" // a2 = *current_task
                     "l32i   a1, a2, 0           \n" // a1 (sp) = current_task->sp
                     "l32i   a0, a1, 64          \n" // a0 = EPC1 (entry point)
                     "addi   a1, a1, 72          \n" // pop the frame
                     "ret                        \n" // jump to entry
                     ::
                         : "a0", "a1", "a2");

    __builtin_unreachable();
}