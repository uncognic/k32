.section .iram.text, "ax"
.align 4
.global _kernel_timer_isr
.global current_task
.global scheduler_tick

_kernel_timer_isr:
    # save context onto stack
    addi a1, a1, -72

    s32i a0, a1, 0 # a0 return addr 
    rsr a0, SAR # save SAR

    # save registers
    s32i a0, a1, 4
    s32i a2, a1, 8
    s32i a3, a1, 12
    s32i a4, a1, 16
    s32i a5, a1, 20
    s32i a6, a1, 24
    s32i a7, a1, 28
    s32i a8, a1, 32
    s32i a9, a1, 36
    s32i a10, a1, 40
    s32i a11, a1, 44
    s32i a12, a1, 48
    s32i a13, a1, 52
    s32i a14, a1, 56
    s32i a15, a1, 60

    rsr a0, EPC_1 # save EPC_1
    s32i a0, a1, 64 # interrupted pc

    rsr a0, EPS_2 # save EPS_2
    s32i a0, a1, 68 # interrupted ps

    # current_task->sp = a1
    movi a2, current_task
    l32i a2, a2, 0 # a2 = *current_task
    s32i a1, a2, 0 # *current_task->sp = a1

    # pick next task
    call0 scheduler_tick

    # a1 = next task sp
    movi a2, current_task
    l32i a2, a2, 0 # a2 = *current_task
    l32i a1, a2, 0 # a1 = *current_task->sp

    # restore context
    l32i    a0,  a1,  68
    wsr     a0,  EPS_2
    l32i    a0,  a1,  64
    wsr     a0,  EPC_1
    l32i    a15, a1,  60
    l32i    a14, a1,  56
    l32i    a13, a1,  52
    l32i    a12, a1,  48
    l32i    a11, a1,  44
    l32i    a10, a1,  40
    l32i    a9,  a1,  36
    l32i    a8,  a1,  32
    l32i    a7,  a1,  28
    l32i    a6,  a1,  24
    l32i    a5,  a1,  20
    l32i    a4,  a1,  16
    l32i    a3,  a1,  12
    l32i    a2,  a1,  8
    l32i    a0,  a1,  4
    wsr     a0,  SAR
    l32i    a0,  a1,  0 # restore a0 last
    addi    a1,  a1,  72