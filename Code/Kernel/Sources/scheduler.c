#include "../Includes/scheduler.h"

extern void switch_to_task(task *next_task);

extern task* current;

void schedule() {
    if (!current) {
        return;
    }

    // Find the next task that is ready to run
    task* next_task = current->next;

    if (next_task->state == TERMINATED) {
        remove_task(next_task);
        next_task = current->next;
    }

    while (next_task->state != READY && next_task != current) {
        next_task = next_task->next;
    }

    __asm__("cli"); // This needs to be fixed
    switch_to_task(next_task);
    __asm__("sti"); // This needs to be fixed

    // CLI/STI critical section probably unneeded if you modify your interrupt
    // handler to save the CursorPosition at the start and restore it before returning
    // I have to investigate.
    __asm__("cli");
    setCursorPosition(8, 0);
    printf("Current process: %d | sp: %p | cp: %p", COLOR_BLACK_ON_WHITE, current->pid,current->sp,current->pc );
    __asm__("sti");
}

void init_scheduler(void) {
    current = (task*)malloc(sizeof(task));
    // Create a PCB for the main task
    if (current) {
        current->pid = new_pid();
        current->state = READY;
        current->flags = 0;
        current->pc = 0;
        current->sp = 0;
        current->base_sp = 0;
        current->esp0 = 0;
        current->ss0 = 0;
        current->next = current;
        nowTasks++;
    }

    pit_init(1);  // Set the PIT to 100Hz, or every 10ms
}

void yield(void) {
    schedule();
}
