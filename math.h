#pragma once
#include <random>

namespace math
{
    template <typename T>
    static T get_random(T min, T max)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        const std::uniform_real_distribution<> dis(min, max);

        return dis(gen);
    }


    static float get_st_dev(const std::vector<std::pair<int, int>>& generated_delays)
    {
        auto sum = 0.f;
        auto st_dev = 0.f;

        for (auto& [first, second] : generated_delays)
        {
            sum += first;
        }

        const auto mean = sum / generated_delays.size();

        for (auto& [first, second] : generated_delays)
        {
            st_dev += pow(first - mean, 2);
        }

        return sqrt(st_dev / generated_delays.size());
    };
}
