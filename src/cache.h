#pragma once

#include "memreq.h"

#include "memory.h"
#include "accelerator.h"

#include <queue>
using std::queue;

#include <utility>
using std::pair;

class Accelerator;
class Memory;

class Cache {
public:
    Cache(Accelerator* _accel);
    void load(MemReq* req);
    void store(MemReq* req);
    void next_cycle();
private:
    Accelerator* accel;
    queue<pair<MemReq*,uint64_t>> memreqs;
    void access(MemReq* req);
};