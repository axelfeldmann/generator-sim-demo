#pragma once

#include "task.h"

#include <memory>
using std::unique_ptr;

class PE {
public:
    bool idle() const;
    void issue(unique_ptr<Task>&& task);
    void next_cycle();
private:
    uint64_t cycles_remaining = 0;
    unique_ptr<Task> cur_task = nullptr;
};
