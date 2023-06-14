#pragma once

#include "generator.h"
#include "instr.h"
#include "matrixmarket.h"

#include <memory>
using std::unique_ptr;

#include <vector>
using std::vector;

Generator<Instruction*> spmv(const char* mtx_file) {

    auto csr = read_matrix_market(mtx_file);
    auto x = vector<double>(csr.cols, 1.0);
    auto b = vector<double>(csr.rows, 0.0);
    
    for (uint64_t row = 0; row < csr.rows; row++) {

        uint64_t row_start = csr.row_pointers.at(row);
        uint64_t row_end = csr.row_pointers.at(row + 1);

        double* dest = &b.at(row);

        co_yield new LoadAccumulator(dest);

        for (uint64_t idx = row_start; idx < row_end; idx++) {

            uint64_t col = csr.columns.at(idx);
            
            double* src1 = &csr.values.at(idx);
            double* src2 = &x.at(col);

            co_yield new FMAC(dest, src1, src2);
        }

        co_yield new StoreAccumulator(dest);
    }
}
