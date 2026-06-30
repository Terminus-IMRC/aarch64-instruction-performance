# [`aarch64-instruction-performance`](https://github.com/Terminus-IMRC/aarch64-instruction-performance)

This repository provides benchmark programs for measuring AArch64 instruction
performance (throughput and latency).


## Dependencies

The benchmark programs use the following libraries, which are automatically
downloaded and built by the build system:

- [fujitsu/xbyak_aarch64](https://github.com/fujitsu/xbyak_aarch64) to generate AArch64 instructions at runtime.
- [google/benchmark](https://github.com/google/benchmark) for easy benchmarking and pretty reporting.
- [lemire/counters](https://github.com/lemire/counters) to access the hardware performance counters.


## Building and running

```console
$ cmake -S . -B build/
$ cmake --build build/
$ # Running benchmarks requires root privileges to access the performance counters.
$ sudo ./build/benchmark-neon  # Below is an example output on Apple M4.
...
-------------------------------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations cycles_per_instruction
-------------------------------------------------------------------------------------------------------------
benchmark_code<Add2dKernel<1>>/10000000      145878317 ns    145863400 ns            5                2.00313
benchmark_code<Add2dKernel<32>>/10000000      19019679 ns     19019316 ns           38               0.250491
benchmark_code<Fmla2dKernel<1>>/10000000     222397389 ns    222392667 ns            3                3.06185
benchmark_code<Fmla2dKernel<32>>/10000000     19523746 ns     19523447 ns           38               0.251714
benchmark_code<Frintx2dKernel<1>>/10000000   218093500 ns    218082000 ns            3                3.00323
benchmark_code<Frintx2dKernel<32>>/10000000   19008856 ns     19008289 ns           38               0.250571
benchmark_code<Mla4sKernel<1>>/10000000      218084361 ns    218080333 ns            3                3.00401
benchmark_code<Mla4sKernel<32>>/10000000      19929218 ns     19892000 ns           36                0.25499
benchmark_code<Pmul16bKernel<1>>/10000000    220214181 ns    220170000 ns            3                 3.0083
benchmark_code<Pmul16bKernel<32>>/10000000    19460493 ns     19460139 ns           36                0.25148
benchmark_code<Sdot16bKernel<1>>/10000000    227089764 ns    226684667 ns            3                3.00075
benchmark_code<Sdot16bKernel<32>>/10000000    20163756 ns     20136429 ns           35               0.255722
benchmark_code<Umlal2dKernel<1>>/10000000    221976958 ns    221717667 ns            3                3.00073
benchmark_code<Umlal2dKernel<32>>/10000000    20165595 ns     20148200 ns           35               0.255457
```


## License and contribution

For license and copyright notices, see the SPDX file tags in each file.
Unless otherwise noted, files in this project are licensed under the Apache
License, Version 2.0 (SPDX short-form identifier: Apache-2.0) and copyrighted by
the contributors.

Everyone is encouraged to contribute to this project.
See the [CONTRIBUTING.md](CONTRIBUTING.md) file for instructions.
