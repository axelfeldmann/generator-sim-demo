#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <algorithm>

using std::vector;
using std::ifstream;
using std::istringstream;
using std::string;
using std::sort;
using std::cerr;
using std::endl;

// Definition of the csr_matrix struct
struct csr_matrix {
    vector<double> values;
    vector<uint64_t> columns;
    vector<uint64_t> row_pointers;
    uint64_t rows;
    uint64_t cols;

    csr_matrix(uint64_t rows, uint64_t cols) : rows(rows), cols(cols) {
        row_pointers.resize(rows + 1); 
    }
};

// Helper struct for COO format
struct coo_entry {
    uint64_t row;
    uint64_t col;
    double val;
};

// Function to read Matrix Market formatted file
csr_matrix read_matrix_market(const char* mtx_file) {
    ifstream file(mtx_file);
    if (!file.is_open()) {
        cerr << "Could not open file: " << mtx_file << endl;
        exit(EXIT_FAILURE);
    }

    string line;
    // Skip comments and read header
    do {
        getline(file, line);
    } while(line[0] == '%');

    istringstream s(line);
    uint64_t M, N, nnz;
    s >> M >> N >> nnz;

    vector<coo_entry> coo_entries;
    coo_entries.reserve(nnz);

    uint64_t row, col;
    double val;
    while (file >> row >> col >> val) {
        --row;  // Convert 1-based index to 0-based
        --col;  
        coo_entries.push_back(coo_entry{row, col, val});
    }

    // Sort COO entries by row and then column
    sort(coo_entries.begin(), coo_entries.end(), 
        [](const coo_entry& a, const coo_entry& b) -> bool {
            return a.row < b.row || (a.row == b.row && a.col < b.col);
    });

    csr_matrix csr_matrix(M, N);

    // Convert sorted COO to CSR format
    uint64_t current_row = -1;
    for (const auto& entry : coo_entries) {
        if(entry.row != current_row){
            csr_matrix.row_pointers[entry.row] = csr_matrix.values.size();
            current_row = entry.row;
        }
        csr_matrix.values.push_back(entry.val);
        csr_matrix.columns.push_back(entry.col);
    }

    // Fill the rest of the row_pointer array
    for (uint64_t r = current_row + 1; r <= M; ++r) {
        csr_matrix.row_pointers[r] = nnz;
    }

    return csr_matrix;
}
