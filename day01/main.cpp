#include <fmt/core.h>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

int main()
{
    const auto elves{[]() {
        std::string line{};
        std::vector<std::vector<size_t>> res{std::vector<size_t>{}};

        while (std::getline(std::cin, line)) {
            if (!line.empty()) {
                res.back().emplace_back(std::stol(line));
            } else {
                res.emplace_back();
            }
        }

        return res;
    }()};

    const auto [part1, part2]{[&]() {
        const auto sortedCalories{[&]() {
            std::vector<size_t> res(elves.size(), size_t{0});
            std::transform(std::cbegin(elves), std::cend(elves), std::begin(res), [](const auto& elf) {
                return std::accumulate(std::cbegin(elf), std::cend(elf), size_t{0});
            });
            std::sort(std::begin(res), std::end(res), std::greater{});
            return res;
        }()};

        return std::tuple<size_t, size_t>{sortedCalories[0], sortedCalories[0] + sortedCalories[1] + sortedCalories[2]};
    }()};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
