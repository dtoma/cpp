Compare:
- sorted vector of pairs + binary search
- hashmap
- map

In the output, the first number if the key we search for, the second one is the number of elements.

```
dtoma@dtoma array_vs_map()$ make main && ./main
g++ -Wall -Wextra -std=c++17 -march=native -O3 -flto -isystem benchmark/include  -Lbuild/src -flto  main.cpp  -lbenchmark -lpthread -o main
------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations
------------------------------------------------------------------
bench_vector/5/100            5.26 ns         5.26 ns    115255856
bench_vector/50/100           5.73 ns         5.73 ns    116477509
bench_vector/95/100           5.38 ns         5.35 ns    125022459
bench_vector/5/10000          9.55 ns         9.54 ns     71435035
bench_vector/5000/10000       9.84 ns         9.83 ns     68829323
bench_vector/9995/10000       9.52 ns         9.52 ns     71185929
------------------------------------------------------------------
bench_umap/5/100              8.84 ns         8.83 ns     77563796
bench_umap/50/100             8.88 ns         8.88 ns     76509034
bench_umap/95/100             9.25 ns         9.25 ns     77883924
bench_umap/5/10000            8.84 ns         8.84 ns     77987990
bench_umap/5000/10000         8.83 ns         8.83 ns     77694641
bench_umap/9995/10000         8.81 ns         8.80 ns     78163208
------------------------------------------------------------------
bench_map/5/100               3.70 ns         3.70 ns    191032225
bench_map/50/100              6.40 ns         6.40 ns    104937583
bench_map/95/100              6.18 ns         6.18 ns    109421998
bench_map/5/10000             8.09 ns         8.07 ns     84249914
bench_map/5000/10000          12.0 ns         12.0 ns     58726094
bench_map/9995/10000          53.7 ns         53.7 ns     12374870
```

```bash
cat bench.json | jq '.benchmarks | .[] | {name: .name, time: .real_time}' > data.json
```