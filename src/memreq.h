#pragma once

#include <cstdint>

struct MemReq {
    const uintptr_t addr;
    const bool is_store;
    bool done;

    explicit MemReq(uintptr_t _addr, bool _is_store = false) : 
        addr(_addr), is_store(_is_store), done(false) {}

    void complete() {
        done = true;
    }
};