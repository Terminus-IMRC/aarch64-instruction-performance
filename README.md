# [`aarch64-instruction-performance`](https://github.com/Terminus-IMRC/aarch64-instruction-performance)

This repository provides benchmark programs for measuring AArch64 instruction
performance (throughput and latency).


## Dependencies

The benchmark programs use the following libraries, which are automatically
downloaded and built by the build system:

- [google/cpu_features](https://github.com/google/cpu_features) to detect processor capabilities at runtime.
- [fujitsu/xbyak_aarch64](https://github.com/fujitsu/xbyak_aarch64) to generate AArch64 instructions at runtime.
- [google/benchmark](https://github.com/google/benchmark) for easy benchmarking and pretty reporting.
- [lemire/counters](https://github.com/lemire/counters) to access the hardware performance counters.


## Building and running

```console
$ cmake -S . -B build/
$ cmake --build build/
$ # Running benchmarks may require root privileges to access the performance counters.
$ sudo ./build/benchmark-neon --benchmark_counters_tabular=true  # Below is an example output on Apple M4.
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
$ sudo ./build/benchmark-sve --benchmark_counters_tabular=true
...
----------------------------------------------------------------------------------------------------------------
Benchmark                       Time             CPU   Iterations cycles_per_instruction group_size is_streaming
----------------------------------------------------------------------------------------------------------------
AddD/1/1/10000000       244575708 ns    244570000 ns            3                 3.0024          1            1
MlaD/1/1/10000000       652430625 ns    652344000 ns            1                7.98577          1            1
UmulhD/1/1/10000000     651991000 ns    651958000 ns            1                7.98878          1            1
UdivD/1/1/10000000     1872461375 ns   1872435000 ns            1                22.8916          1            1
UdotD/1/1/10000000      652092333 ns    652050000 ns            1                7.98804          1            1
FaddD/1/1/10000000      652073417 ns    652023000 ns            1                7.98626          1            1
FmlaD/1/1/10000000      652245667 ns    652196000 ns            1                7.98814          1            1
FdivD/1/1/10000000     1384129333 ns   1384069000 ns            1                16.9395          1            1
FrintxD/1/1/10000000    244424070 ns    244417333 ns            3                3.00236          1            1
FsqrtD/1/1/10000000    2197924750 ns   2197909000 ns            1                26.8688          1            1
FrsqrteD/1/1/10000000   814677375 ns    814653000 ns            1                9.98123          1            1
FrecpeD/1/1/10000000    814536000 ns    814471000 ns            1                   9.98          1            1
AddD/32/1/10000000       81515657 ns     81513444 ns            9                1.00181         32            1
MlaD/32/1/10000000      325928834 ns    325921000 ns            2                4.00147         32            1
UmulhD/32/1/10000000    326081688 ns    326076000 ns            2                4.00331         32            1
UdivD/32/1/10000000    1303156167 ns   1303105000 ns            1                15.9466         32            1
UdotD/32/1/10000000     325960208 ns    325948500 ns            2                4.00225         32            1
FaddD/32/1/10000000     325932187 ns    325922500 ns            2                4.00201         32            1
FmlaD/32/1/10000000     325924875 ns    325914500 ns            2                4.00194         32            1
FdivD/32/1/10000000     651969417 ns    651909000 ns            1                 7.9878         32            1
FrintxD/32/1/10000000    81562875 ns     81558889 ns            9                1.00176         32            1
FsqrtD/32/1/10000000   1302535041 ns   1302482000 ns            1                15.9514         32            1
FrsqrteD/32/1/10000000  652290333 ns    652256000 ns            1                7.98937         32            1
FrecpeD/32/1/10000000   651969709 ns    651895000 ns            1                7.98805         32            1
```


## License and contribution

For license and copyright notices, see the SPDX file tags in each file.
Unless otherwise noted, files in this project are licensed under the Apache
License, Version 2.0 (SPDX short-form identifier: Apache-2.0) and copyrighted by
the contributors.

Everyone is encouraged to contribute to this project.
See the [CONTRIBUTING.md](CONTRIBUTING.md) file for instructions.
