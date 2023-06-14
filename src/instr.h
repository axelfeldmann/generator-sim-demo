#pragma once

#include "memreq.h"

#include <cstdint>

#include <sstream>
using std::stringstream;

#include <string>
using std::string;

struct Instruction {
    virtual bool ready() const = 0;
    virtual ~Instruction() = default;
    virtual string to_string() const = 0;
};

struct LoadAccumulator : public Instruction {
    MemReq dest;
    LoadAccumulator(double* _dest) : dest((uintptr_t)_dest) {}
    bool ready() const { return dest.done; }
    string to_string() const {
        stringstream ss;
        ss << "LoadAccumulator(0x" << std::hex << dest.addr << ")";
        return ss.str();
    }
};

struct FMAC : public Instruction {
    double* dest;
    MemReq src1;
    MemReq src2;

    FMAC(double* _dest, double* _src1, double* _src2) : 
        dest(_dest), src1((uintptr_t) _src1), src2((uintptr_t) _src2) {}
    bool ready() const {
        return src1.done && src2.done; 
    }
    string to_string() const {
        stringstream ss;
        ss << "FMAC(" << dest << ", 0x" << std::hex << src1.addr << ", 0x" << std::hex << src2.addr << ")";
        return ss.str();
    }
};

struct StoreAccumulator : public Instruction {
    MemReq dest;
    StoreAccumulator(double* _dest) : dest((uintptr_t)_dest, true) {}
    bool ready() const { return true; }
    string to_string() const {
        stringstream ss;
        ss << "StoreAccumulator(0x" << std::hex << dest.addr << ")";
        return ss.str();
    }
};