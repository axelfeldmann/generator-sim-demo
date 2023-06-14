#include "instr.h"
#include "accelerator.h"
#include "gemm.h"

#include <cstdio>

int main() {

    Generator<Instruction*> gen = gemm(10, 10, 10);

    Accelerator accel;

    while (gen || !accel.idle()) {

        if (gen && accel.can_schedule()) {
            auto* instr = gen();
            accel.schedule(instr);
        }

        accel.next_cycle();
    }
    
    printf("simulation took %lu cycles\n", accel.cur_cycle);
    return 0;
}