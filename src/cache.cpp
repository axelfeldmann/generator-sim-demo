#include "cache.h"

Cache::Cache(Accelerator* _accel) : accel(_accel) {
}

void Cache::load(MemReq* req) {
    access(req);
    memreqs.push({ req, accel->cur_cycle + 10 });
}

void Cache::store(MemReq* req) {
    access(req);
}

void Cache::access(MemReq* req) {
    memreqs.push({ req, accel->cur_cycle + 10 });
}

void Cache::next_cycle() {
    while (!memreqs.empty() && memreqs.front().second == accel->cur_cycle) {
        auto* req = memreqs.front().first;
        memreqs.pop();
        req->complete();
    }
}