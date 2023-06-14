#pragma once

#include <cassert>

#include <cstdint>
#include <vector>
using std::vector;

#include "instr.h"

struct Gemm {

    uint32_t N;
    uint32_t K;
    uint32_t M;

    vector<vector<double>> A;
    vector<vector<double>> B;
    vector<vector<double>> C;

    Gemm(uint32_t N, uint32_t K, uint32_t M) : N(N), K(K), M(M),
        A(N, vector<double>(K, 1.0)),
        B(K, vector<double>(M, 1.0)),
        C(N, vector<double>(M, 0.0)) {}

    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;

    double* accum = nullptr;

    Instruction* next() {

        assert(i < N);
        assert(j < M);

        auto* dest = &C.at(i).at(j);

        if (dest != accum) {
            accum = dest;
            return new LoadAccumulator(dest);
        }

        assert(dest == accum);
        
        if (k == K) {
            k = 0;
            j++;
            if (j == M) {
                j = 0;
                i++;
            }
            return new StoreAccumulator(dest);
        }

        assert(k < K);

        double* src1 = &A.at(i).at(k);
        double* src2 = &B.at(k).at(j);

        k++;
        return new FMAC(dest, src1, src2);
    }

    bool done() {
        return i == N;
    }

};