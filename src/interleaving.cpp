#include "instr.h"
#include "accelerator.h"
#include "gemm.h"

#include <list>
using std::list;

#include <cassert>

#include <cstdio>

int main() {

    // We're going to interleave two different gemm computations
    Generator<Instruction*> gen1 = gemm(10, 10, 10);
    Generator<Instruction*> gen2 = gemm(10, 10, 10);
    list<Generator<Instruction*>*> gens = { &gen1, &gen2 };

    Accelerator accel;

    // Round robin between the generators in gens
    while (!gens.empty() || !accel.idle()) {
        if (!gens.empty() && accel.can_schedule()) {

            // Take the first generator from the front of the line
            auto* gen = gens.front();
            gens.pop_front();

            // Schedule an instruction
            assert(*gen);
            auto* instr = (*gen)();
            accel.schedule(instr);

            // If the generator still has more instrs, put it at the
            // back of the line (to achieve round robin behavior)
            if (*gen) {
                gens.push_back(gen);
            }
        }

        accel.next_cycle();
    }
    
    printf("simulation took %lu cycles\n", accel.cur_cycle);
    return 0;
}
