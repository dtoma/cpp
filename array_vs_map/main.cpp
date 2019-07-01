#include <algorithm>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>

void bench_vector(benchmark::State& state) {
  std::vector<std::pair<int, int>> v;
  for (int i = 0; i < state.range(1); ++i) {
    v.emplace_back(i, i);
  }
  for (auto _ : state)
    benchmark::DoNotOptimize(std::lower_bound(
        std::begin(v),
        std::end(v),
        std::make_pair(state.range(0), 0),
        [](auto const& a, auto const& b) {
          return a.first == b.first; }
    ));
}

void bench_umap(benchmark::State& state) {
  std::unordered_map<int, int> m;
  for (int i = 0; i < state.range(1); ++i) {
    m.emplace(i, i);
  }
  for (auto _ : state)
    benchmark::DoNotOptimize(m.at(state.range(0)));
}

void bench_map(benchmark::State& state) {
  std::map<int, int> m;
  for (int i = 0; i < state.range(1); ++i) {
    m.emplace(i, i);
  }
  for (auto _ : state)
    benchmark::DoNotOptimize(m.at(state.range(0)));
}

BENCHMARK(bench_vector)
  ->Args({5, 100})
  ->Args({50, 100})
  ->Args({95, 100})
  ->Args({5, 10000})
  ->Args({5000, 10000})
  ->Args({9995, 10000});
BENCHMARK(bench_umap)
  ->Args({5, 100})
  ->Args({50, 100})
  ->Args({95, 100})
  ->Args({5, 10000})
  ->Args({5000, 10000})
  ->Args({9995, 10000});
BENCHMARK(bench_map)
  ->Args({5, 100})
  ->Args({50, 100})
  ->Args({95, 100})
  ->Args({5, 10000})
  ->Args({5000, 10000})
  ->Args({9995, 10000});

BENCHMARK_MAIN();
