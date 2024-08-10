#include <benchmark/benchmark.h>
#include <functional>
#include <vector>
#include <iostream>
#include "paeth.hpp"

inline void benchmark_tempalte(benchmark::State &state, std::function<uint8_t(uint8_t,uint8_t,uint8_t)> paeth) {
    for(auto _: state) {
        state.PauseTiming();
        const int N = state.range(0);
        uint8_t *a, *b, *c, *ans;
        a = (uint8_t *)malloc(N * sizeof(uint8_t));
        b = (uint8_t *)malloc(N * sizeof(uint8_t));
        c = (uint8_t *)malloc(N * sizeof(uint8_t));
        ans = (uint8_t *)malloc(N * sizeof(uint8_t));
        srand((unsigned int)time(NULL));
        for (int i = 0; i < N; i++) a[i] = (uint8_t)rand();
        for (int i = 0; i < N; i++) b[i] = (uint8_t)rand();
        for (int i = 0; i < N; i++) c[i] = (uint8_t)rand();
        state.ResumeTiming();

        for (int i=0; i<N; i++) {
            ans[i] = paeth(a[i], b[i], c[i]);
        }
        benchmark::DoNotOptimize(ans);

        state.PauseTiming();
        free(a);
        free(b);
        free(c);
        free(ans);
        state.ResumeTiming();
    }
}

inline void benchmark_simd(benchmark::State &state, std::function<void(uint8_t*,uint8_t*,uint8_t*,uint8_t*)> paeth) {
    for(auto _: state) {
        state.PauseTiming();
        const int N = state.range(0);
        uint8_t *a, *b, *c, *ans;
        a = (uint8_t *)malloc(N * sizeof(uint8_t));
        b = (uint8_t *)malloc(N * sizeof(uint8_t));
        c = (uint8_t *)malloc(N * sizeof(uint8_t));
        ans = (uint8_t *)malloc(N * sizeof(uint8_t));
        srand((unsigned int)time(NULL));
        for (int i = 0; i < N; i++) a[i] = (uint8_t)rand();
        for (int i = 0; i < N; i++) b[i] = (uint8_t)rand();
        for (int i = 0; i < N; i++) c[i] = (uint8_t)rand();
        state.ResumeTiming();

        for (int i=0; i<N; i+=8) {
            paeth(a+i, b+i, c+i, ans+i);
        }
        benchmark::DoNotOptimize(ans);

        state.PauseTiming();
        free(a);
        free(b);
        free(c);
        free(ans);
        state.ResumeTiming();
    }
}

void BM_SIMPLE(benchmark::State &state) {
    benchmark_tempalte(state, paeth_simple);
}
BENCHMARK(BM_SIMPLE)->Range(1 << 9, 1 << 15);

#ifdef __SSE4_1__
void BM_BEFORE(benchmark::State &state) {
    benchmark_simd(state, paeth_before);
}
void BM_AFTER(benchmark::State &state) {
    benchmark_simd(state, paeth_after);
}
BENCHMARK(BM_BEFORE)->Range(1 << 9, 1 << 15);
BENCHMARK(BM_AFTER)->Range(1 << 9, 1 << 15);
#endif // __SSE4_1__
