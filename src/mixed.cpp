#include "instr.h"
#include "accelerator.h"
#include "spmv.h"

#include <list>
using std::list;

#include <cassert>

#include <cstdio>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: ./build/mixed <matrices...>");
        return 2;
    }

    // We're going to interleave two different gemm computations
    list<Generator<Instruction*>*> gens;
    for (int i = 1; i < argc; i++) {
        auto* gen = new Generator<Instruction*>(spmv(argv[i]));
        gens.push_back(gen);
    }

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
            } else {
                delete gen;
            }
        }
        accel.next_cycle();
    }
    
    printf("simulation took %lu cycles\n", accel.cur_cycle);
    return 0;
}
