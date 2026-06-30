// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright © 2026 Yukimasa Sugizaki

#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include <xbyak_aarch64.h>

#include <counters/event_counter.h>

#include <benchmark/benchmark.h>

static counters::event_collector collector;

struct Boilerplate : Xbyak_aarch64::CodeGenerator {

  Xbyak_aarch64::Label label;

  void prologue(void) {
    using namespace Xbyak_aarch64;

    for (std::size_t i = 0; i < 4; ++i) {
      stp(DReg(8 + i * 2 + 0), DReg(8 + i * 2 + 1), pre_ptr(sp, -16));
    }

    L(label);
  }

  void epilogue(void) {
    using namespace Xbyak_aarch64;

    subs(x0, x0, 1);
    b(NE, label);

    for (std::size_t i = 0; i < 4; ++i) {
      ldp(DReg(15 - i * 2 - 1), DReg(15 - i * 2 - 0), post_ptr(sp, 16));
    }

    ret();
  }
};

template <std::size_t group_size> struct Add2dKernel : Boilerplate {

  Add2dKernel(void) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      add(VReg(j).d, VReg(j).d, VReg(j).d);
    }

    Boilerplate::epilogue();
  }
};

template <std::size_t group_size> struct Fmla2dKernel : Boilerplate {

  Fmla2dKernel(void) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fmla(VReg(j).d, VReg(j).d, VReg(j).d);
    }

    Boilerplate::epilogue();
  }
};

template <std::size_t group_size> struct Frintx2dKernel : Boilerplate {

  Frintx2dKernel(void) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      frintx(VReg(j).d, VReg(j).d);
    }

    Boilerplate::epilogue();
  }
};

template <std::size_t group_size> struct Mla4sKernel : Boilerplate {

  Mla4sKernel(void) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      mla(VReg(j).s, VReg(j).s, VReg(j).s);
    }

    Boilerplate::epilogue();
  }
};

template <std::size_t group_size> struct Pmul16bKernel : Boilerplate {

  Pmul16bKernel(void) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      pmul(VReg(j).b, VReg(j).b, VReg(j).b);
    }

    Boilerplate::epilogue();
  }
};

template <std::size_t group_size> struct Sdot16bKernel : Boilerplate {

  Sdot16bKernel(void) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      sdot(VReg(j).s, VReg(j).b, VReg(j).b);
    }

    Boilerplate::epilogue();
  }
};

template <std::size_t group_size> struct Umlal2dKernel : Boilerplate {

  Umlal2dKernel(void) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      umlal(VReg(j).d, VReg(j).s2, VReg(j).s2);
    }

    Boilerplate::epilogue();
  }
};

template <class code_type> static void benchmark_code(benchmark::State &state) {
  code_type code;
  code.ready();
  const auto function =
      code.template getCode<void (*)(std::uint64_t loop_count)>();

  const std::uint64_t loop_count = state.range(0);

  if (state.thread_index() == 0) {
    collector.start();
  }

  for (auto _ : state) {
    function(loop_count);
  }

  if (state.thread_index() == 0) {
    const counters::event_count &count = collector.end();

    const std::uint64_t num_iterations = loop_count * state.iterations();
    state.counters["cycles_per_instruction"] =
        count.cycles() / (count.instructions() - 2. * num_iterations);
  }
}

int main(int argc, char *argv[]) {
  if (!collector.has_events()) {
    throw std::runtime_error("failed to enable performance counters");
  }

  benchmark::MaybeReenterWithoutASLR(argc, argv);
  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}

BENCHMARK(benchmark_code<Add2dKernel<1>>)->Arg(1e7);
BENCHMARK(benchmark_code<Add2dKernel<32>>)->Arg(1e7);
BENCHMARK(benchmark_code<Fmla2dKernel<1>>)->Arg(1e7);
BENCHMARK(benchmark_code<Fmla2dKernel<32>>)->Arg(1e7);
BENCHMARK(benchmark_code<Frintx2dKernel<1>>)->Arg(1e7);
BENCHMARK(benchmark_code<Frintx2dKernel<32>>)->Arg(1e7);
BENCHMARK(benchmark_code<Mla4sKernel<1>>)->Arg(1e7);
BENCHMARK(benchmark_code<Mla4sKernel<32>>)->Arg(1e7);
BENCHMARK(benchmark_code<Pmul16bKernel<1>>)->Arg(1e7);
BENCHMARK(benchmark_code<Pmul16bKernel<32>>)->Arg(1e7);
BENCHMARK(benchmark_code<Sdot16bKernel<1>>)->Arg(1e7);
BENCHMARK(benchmark_code<Sdot16bKernel<32>>)->Arg(1e7);
BENCHMARK(benchmark_code<Umlal2dKernel<1>>)->Arg(1e7);
BENCHMARK(benchmark_code<Umlal2dKernel<32>>)->Arg(1e7);
