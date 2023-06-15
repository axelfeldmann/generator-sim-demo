#pragma once

#include "instr.h"
#include "cache.h"

#include <memory>
using std::unique_ptr;

#include <queue>
using std::queue;

class Cache;

class Accelerator {
public:
    Accelerator();
    bool can_schedule() const;
    bool idle() const;
    void schedule(Instruction* instr);
    void next_cycle();
    uint64_t cur_cycle = 0;

    ~Accelerator();
private:
    queue<Instruction*> fifo;
    const uint32_t FIFO_CAPACITY = 8;
    
    double* accumulator;

    Instruction* executing = nullptr;
    uint64_t done_cycle;

    Cache* cache;
    queue<StoreAccumulator*> store_queue;
};
