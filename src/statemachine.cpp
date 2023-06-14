#include "instr.h"
#include "accelerator.h"
#include "next.h"

#include <cstdio>

int main() {

    Gemm program(10, 10, 10);

    Accelerator accel;

    while (!program.done() || !accel.idle()) {

        if (!program.done() && accel.can_schedule()) {
            auto* instr = program.next();
            accel.schedule(instr);
        }

        accel.next_cycle();
    }
    
    printf("simulation took %lu cycles\n", accel.cur_cycle);
    return 0;
}