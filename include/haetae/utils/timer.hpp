#pragma once

#include <chrono>
#include <functional>
#include <type_traits>
#include <utility>

template<typename T, typename DurationUnit = std::chrono::microseconds>
struct TimedResult {
    T result;
    DurationUnit duration;
};

template<typename DurationUnit>
struct TimedResult<void, DurationUnit> {
    DurationUnit duration;
};

template<typename DurationUnit = std::chrono::microseconds, typename Fn, typename... Args>
inline auto time_execution(Fn&& fn, Args&&... args) {
    using ReturnType = std::invoke_result_t<Fn, Args...>;
    const auto start = std::chrono::steady_clock::now();

    if constexpr(std::is_void_v<ReturnType>) {
        std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
        
        const auto end = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration_cast<DurationUnit>(end - start);
        return TimedResult<void, DurationUnit> { diff };
    } else {
        auto result = std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
        
        const auto end = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration_cast<DurationUnit>(end - start);
        return TimedResult<ReturnType, DurationUnit> { std::move(result), diff };
    }
}