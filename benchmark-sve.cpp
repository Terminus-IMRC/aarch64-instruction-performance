// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright © 2026 Yukimasa Sugizaki

#include <cstddef>
#include <cstdint>
#include <stdexcept>

#if defined(__APPLE__)
#include <sys/sysctl.h>
#include <sys/types.h>
#endif /* defined(__APPLE__) */

#include <cpuinfo_aarch64.h>

#include <xbyak_aarch64.h>

#include <counters/event_counter.h>

#include <benchmark/benchmark.h>

static counters::event_collector collector;

struct Boilerplate : Xbyak_aarch64::CodeGenerator {

  Xbyak_aarch64::Label label;

  void prologue(const bool is_streaming) {
    using namespace Xbyak_aarch64;

    for (std::size_t i = 0; i < 4; ++i) {
      stp(DReg(8 + i * 2 + 0), DReg(8 + i * 2 + 1), pre_ptr(sp, -16));
    }

    if (is_streaming) {
      smstart();
    }

    ptrue(p0.b, Pattern::ALL);

    L(label);
  }

  void epilogue(const bool is_streaming) {
    using namespace Xbyak_aarch64;

    subs(x0, x0, 1);
    b(NE, label);

    if (is_streaming) {
      smstop();
    }

    for (std::size_t i = 0; i < 4; ++i) {
      ldp(DReg(15 - i * 2 - 1), DReg(15 - i * 2 - 0), post_ptr(sp, 16));
    }

    ret();
  }
};

struct AddSKernel : Boilerplate {

  AddSKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      add(ZReg(j).s, ZReg(j).s, ZReg(j).s);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct AddDKernel : Boilerplate {

  AddDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      add(ZReg(j).d, ZReg(j).d, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct MlaSKernel : Boilerplate {

  MlaSKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      mla(ZReg(j).s, p0 / T_z, ZReg(j).s, ZReg(j).s);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct MlaDKernel : Boilerplate {

  MlaDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      mla(ZReg(j).d, p0 / T_z, ZReg(j).d, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct UmulhSKernel : Boilerplate {

  UmulhSKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      umulh(ZReg(j).s, ZReg(j).s, ZReg(j).s);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct UmulhDKernel : Boilerplate {

  UmulhDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      umulh(ZReg(j).d, ZReg(j).d, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct UdivSKernel : Boilerplate {

  UdivSKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      udiv(ZReg(j).s, p0 / T_z, ZReg(j).s);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct UdivDKernel : Boilerplate {

  UdivDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      udiv(ZReg(j).d, p0 / T_z, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct UdotDKernel : Boilerplate {

  UdotDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      udot(ZReg(j).d, ZReg(j).h, ZReg(j).h);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FaddSKernel : Boilerplate {

  FaddSKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fadd(ZReg(j).s, ZReg(j).s, ZReg(j).s);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FaddDKernel : Boilerplate {

  FaddDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fadd(ZReg(j).d, ZReg(j).d, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FmlaSKernel : Boilerplate {

  FmlaSKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fmla(ZReg(j).s, p0 / T_z, ZReg(j).s, ZReg(j).s);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FmlaDKernel : Boilerplate {

  FmlaDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fmla(ZReg(j).d, p0 / T_z, ZReg(j).d, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FdivSKernel : Boilerplate {

  FdivSKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fdiv(ZReg(j).s, p0 / T_z, ZReg(j).s);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FdivDKernel : Boilerplate {

  FdivDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fdiv(ZReg(j).d, p0 / T_z, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FrintxSKernel : Boilerplate {

  FrintxSKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      frintx(ZReg(j).s, p0 / T_z, ZReg(j).s);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FrintxDKernel : Boilerplate {

  FrintxDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      frintx(ZReg(j).d, p0 / T_z, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FsqrtDKernel : Boilerplate {

  FsqrtDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      fsqrt(ZReg(j).d, p0 / T_z, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FrsqrteDKernel : Boilerplate {

  FrsqrteDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      frsqrte(ZReg(j).d, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

struct FrecpeDKernel : Boilerplate {

  FrecpeDKernel(const std::size_t group_size, const bool is_streaming) {
    using namespace Xbyak_aarch64;

    Boilerplate::prologue(is_streaming);

    for (std::size_t i = 0; i < 32 / group_size * group_size; ++i) {
      const std::size_t j = i % group_size;
      frecpe(ZReg(j).d, ZReg(j).d);
    }

    Boilerplate::epilogue(is_streaming);
  }
};

template <class code_type> static void benchmark_code(benchmark::State &state) {
  const std::size_t group_size = state.range(0);
  const bool is_streaming = state.range(1);
  const std::uint64_t loop_count = state.range(2);

  code_type code(group_size, is_streaming);
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
    state.counters["is_streaming"] = is_streaming;
    state.counters["cycles_per_instruction"] =
        count.cycles() / (count.instructions() - 2. * num_iterations);
  }
}

#if defined(__APPLE__)
static bool sysctlbyname_bool(const char *const name) {
  int result;
  std::size_t length = sizeof(result);
  const int ret = sysctlbyname(name, &result, &length, nullptr, 0);

  return ret ? false : result;
}
#endif /* defined(__APPLE__) */

int main(int argc, char *argv[]) {
  if (!collector.has_events()) {
    throw std::runtime_error("failed to enable performance counters");
  }

  benchmark::MaybeReenterWithoutASLR(argc, argv);

  cpu_features::Aarch64Features features =
      cpu_features::GetAarch64Info().features;
#if defined(__APPLE__)
  /*
   * This sysctl feature is not yet incorporated into cpu_features seemingly
   * because it is also missing from the official document:
   * https://developer.apple.com/documentation/kernel/1387446-sysctlbyname/determining_instruction_set_characteristics
   */
  features.sme |= sysctlbyname_bool("hw.optional.arm.FEAT_SME");
#endif /* defined(__APPLE__) */

  for (const bool is_streaming : {false, true}) {
    if (is_streaming ? !features.sme : !features.sve) {
      continue;
    }

    benchmark::RegisterBenchmark("AddS", benchmark_code<AddSKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("AddD", benchmark_code<AddDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("MlaS", benchmark_code<MlaSKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("MlaD", benchmark_code<MlaDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("UmulhS", benchmark_code<UmulhSKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("UmulhD", benchmark_code<UmulhDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("UdivS", benchmark_code<UdivSKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("UdivD", benchmark_code<UdivDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("UdotD", benchmark_code<UdotDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});

    benchmark::RegisterBenchmark("FaddS", benchmark_code<FaddSKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FaddD", benchmark_code<FaddDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FmlaS", benchmark_code<FmlaSKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FmlaD", benchmark_code<FmlaDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FdivS", benchmark_code<FdivSKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FdivD", benchmark_code<FdivDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FrintxS", benchmark_code<FrintxSKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FrintxD", benchmark_code<FrintxDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FsqrtD", benchmark_code<FsqrtDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FrsqrteD", benchmark_code<FrsqrteDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
    benchmark::RegisterBenchmark("FrecpeD", benchmark_code<FrecpeDKernel>)
        ->Args({1, is_streaming, 1'000'000})
        ->Args({2, is_streaming, 1'000'000})
        ->Args({4, is_streaming, 1'000'000})
        ->Args({32, is_streaming, 1'000'000});
  }

  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}
