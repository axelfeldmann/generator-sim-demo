#pragma once

#include "generator.h"
#include "instr.h"

#include <vector>
using std::vector;

Generator<Instruction*> gemm(uint32_t N, uint32_t M, uint32_t K) {

    vector<vector<double>> A(N, vector<double>(K, 1.0));
    vector<vector<double>> B(K, vector<double>(M, 1.0));
    vector<vector<double>> C(N, vector<double>(M, 0.0));

    for (uint32_t i = 0; i < N; i++) {
        for (uint32_t j = 0; j < M; j++) {
            double* dest = &C.at(i).at(j);
            co_yield new LoadAccumulator(dest);
            for (uint32_t k = 0; k < K; k++) {
                double* src1 = &A.at(i).at(k);
                double* src2 = &B.at(k).at(j);
                co_yield new FMAC(dest, src1, src2);
            }
            co_yield new StoreAccumulator(dest);
        }
    }
}
