# C++ Generators for Simulating Hardware Accelerators

This repo showcases the usefulness of generators in writing
simulators for hardware accelerator architecutres.

The overall setup is something like this: we have a class
`Accelerator` that models a basic hardware accelerator
with a 3-instruction ISA: `LoadAccumulator`, `FMAC`, and
`StoreAccumulator`. We want to simulate running various simple
matrix operations such as `GEMM` on this accelerator.

A simple implementation of this can be seen in `statemachine.cpp`

```
Gemm program(10, 10, 10);

Accelerator accel;

while (!program.done() || !accel.idle()) {

    if (!program.done() && accel.can_schedule()) {
        auto* instr = program.next();
        accel.schedule(instr);
    }

    accel.next_cycle();
}
```

We instantiate a `Gemm` program and an `Accelerator` object,
then just keep calling `program.next()` to generate the
next instruction in the program until we're done.

The problem is: *how do we implement `Gemm::next()`*?

## A bad solution

`Gemm::next()` is implemented in `next.h`. It is implemented
as a state machine with state variables `i`, `j`, `k`. The
state machine implements matrix multiplication roughly
as follows:

```
for (uint32_t i = 0; i < N; i++) {
    for (uint32_t j = 0; j < M; j++) {
        for (uint32_t k = 0; k < K; k++) {
            C[i,j] += A[i,k] * B[k,j];
        }
    }
}
```

Every time `program.next()` is called, we advance the state
machine and yield a new instruction. The code in `next.h`
is super nasty and hard to modify:

```
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
```


For example, what if
we wanted to change up the loop order? We'd have to
write a whole new state machine. Also, if we wanted to implement
a more complicated algorithm, converting it to a state
machine would be a pain.

## C++ Generators

Instead, we can turn to the implementation shown in
`gemm.h` (called from `singlegen.cpp`). Instead of implementing
`next()` as a state machine, we can use a `Generator` (built on 
top of C++20 coroutines) to implement the program much more naturally.

Here is the entire code:

```
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
```

Every time `singlegen.cpp` calls `gen()`, the generator resumes,
advances to the next `co_yield`, suspends, and yields an instruction.
This allows us to write programs for our accelerator as *normal
code*. Changing loop orders and implementing new algorithms is
relatively easy. See `spmv.h` for a generator based implementation
of sparse matrix-vector multiplication.

## Additional Benefits

We don't need to have just *one* generator in our simulation!

Instead, as shown in `interleaved.cpp` (and `mixed.cpp`), we
can interleave multiple generators to simulate multi-threading.
Generators are as composable as you want them to be: you can
have generators of generators, you can store generators
in a list, etc.
