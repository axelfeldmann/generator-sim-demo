#include "accelerator.h"

#include <cassert>

Accelerator::Accelerator() {
    cache = new Cache(this);
}

bool Accelerator::can_schedule() const {
    assert(fifo.size() <= FIFO_CAPACITY);
    return fifo.size() < FIFO_CAPACITY;
}

bool Accelerator::idle() const {
    return fifo.empty() && (executing == nullptr) && (store_queue.empty());
}

void Accelerator::schedule(Instruction* instr) {
    assert(can_schedule());

    if (auto load = dynamic_cast<LoadAccumulator*>(instr)) {
        
        cache->load(&load->dest);

    } else if (auto fmac = dynamic_cast<FMAC*>(instr)) {

        cache->load(&fmac->src1);
        cache->load(&fmac->src2);

    } else if (auto store = dynamic_cast<StoreAccumulator*>(instr)) {

        (void) store;

    } else {
        assert(false);
    }

    fifo.push(instr);
}

void Accelerator::next_cycle() {
    cur_cycle++;

    cache->next_cycle();

    // We are executing an instruction, let's see if it's done this cycle
    if (executing != nullptr) {
        if (cur_cycle == done_cycle) {

            // StoreAccumulator instructions have a writeback mechanism
            if (auto store = dynamic_cast<StoreAccumulator*>(executing)) {
                cache->store(&store->dest);

                // We need to wait for the store to complete before
                // deleting the store instruction
                store_queue.push(store);

            } else {

                // We're done with all other instrucitons forever
                delete executing;
            }
            executing = nullptr;
        }
    }

    // If we're not executing anything, let's see if we can issue the next instr
    if (executing == nullptr && !fifo.empty() && fifo.front()->ready()) {
        auto* instr = fifo.front();
        fifo.pop();

        executing = instr;
        done_cycle = cur_cycle + 1;

        printf("[%lu] Issued instr: %s\n", cur_cycle, instr->to_string().c_str());
    }

    // Handle the store queue to avoid memory leaks
    while (!store_queue.empty() && store_queue.front()->dest.done) {
        delete store_queue.front();
        store_queue.pop();
    }
}

Accelerator::~Accelerator() {
    assert(idle());
    delete cache;
}