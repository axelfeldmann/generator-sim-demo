#pragma once

#include <cstdint>

struct FMAC {
    double* dest;
    double* src1;
    double* src2;
    FMAC(double* _dest, double* _src1, double* _src2) : dest(_dest), src1(_src1), src2(_src2) {}
};