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

struct Add2dKernel : Boilerplate {

  Add2dKernel(const std::size_t group_size) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      add(VReg(j).d, VReg(j).d, VReg(j).d);
    }

    Boilerplate::epilogue();
  }
};

struct Fmla2dKernel : Boilerplate {

  Fmla2dKernel(const std::size_t group_size) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fmla(VReg(j).d, VReg(j).d, VReg(j).d);
    }

    Boilerplate::epilogue();
  }
};

struct Frintx2dKernel : Boilerplate {

  Frintx2dKernel(const std::size_t group_size) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      frintx(VReg(j).d, VReg(j).d);
    }

    Boilerplate::epilogue();
  }
};

struct Mla4sKernel : Boilerplate {

  Mla4sKernel(const std::size_t group_size) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      mla(VReg(j).s, VReg(j).s, VReg(j).s);
    }

    Boilerplate::epilogue();
  }
};

struct Pmul16bKernel : Boilerplate {

  Pmul16bKernel(const std::size_t group_size) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      pmul(VReg(j).b, VReg(j).b, VReg(j).b);
    }

    Boilerplate::epilogue();
  }
};

struct Sdot16bKernel : Boilerplate {

  Sdot16bKernel(const std::size_t group_size) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue();

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      sdot(VReg(j).s, VReg(j).b, VReg(j).b);
    }

    Boilerplate::epilogue();
  }
};

struct Umlal2dKernel : Boilerplate {

  Umlal2dKernel(const std::size_t group_size) {
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
  const std::size_t group_size = state.range(0);
  const std::uint64_t loop_count = state.range(1);

  code_type code(group_size);
  code.ready();
  const auto function =
      code.template getCode<void (*)(std::uint64_t loop_count)>();

  if (state.thread_index() == 0) {
    collector.start();
  }

  for (auto _ : state) {
    function(loop_count);
  }

  if (state.thread_index() == 0) {
    const counters::event_count &count = collector.end();

    const std::uint64_t num_iterations = loop_count * state.iterations();
    state.counters["group_size"] = group_size;
    state.counters["cycles_per_instruction"] =
        count.cycles() / (count.instructions() - 2. * num_iterations);
  }
}

int main(int argc, char *argv[]) {
  if (!collector.has_events()) {
    throw std::runtime_error("failed to enable performance counters");
  }

  benchmark::MaybeReenterWithoutASLR(argc, argv);

  for (const std::size_t group_size : {1, 32}) {
    const std::vector<std::int64_t> args{static_cast<std::int64_t>(group_size),
                                         static_cast<std::int64_t>(1e7)};

    benchmark::RegisterBenchmark("Add2d", benchmark_code<Add2dKernel>)
        ->Args(args);
    benchmark::RegisterBenchmark("Fmla2d", benchmark_code<Fmla2dKernel>)
        ->Args(args);
    benchmark::RegisterBenchmark("Frintx2d", benchmark_code<Frintx2dKernel>)
        ->Args(args);
    benchmark::RegisterBenchmark("Mla4s", benchmark_code<Mla4sKernel>)
        ->Args(args);
    benchmark::RegisterBenchmark("Pmul16b", benchmark_code<Pmul16bKernel>)
        ->Args(args);
    benchmark::RegisterBenchmark("Sdot16b", benchmark_code<Sdot16bKernel>)
        ->Args(args);
    benchmark::RegisterBenchmark("Umlal2d", benchmark_code<Umlal2dKernel>)
        ->Args(args);
  }

  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}
