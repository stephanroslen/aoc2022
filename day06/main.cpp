#include <aoclib/CT.h>

#include <fmt/core.h>

#include <cassert>
#include <iostream>
#include <string>
#include <unordered_set>

int main()
{
    const auto input{[]() {
        std::string res{};
        std::cin >> res;
        return res;
    }()};

    const auto solution{[&](const auto cMarkerSize) {
        constexpr auto markerSize{decltype(cMarkerSize)::value};
        for (auto [i, ib, ie]{std::make_tuple(markerSize, std::cbegin(input), std::cbegin(input) + markerSize)};
             ie != std::cend(input);
             ++i, ++ib, ++ie)
        {
            const std::unordered_set<char> charSet{ib, ie};
            if (charSet.size() == markerSize) {
                return i;
            }
        }
        std::terminate();
    }};

    const auto part1{solution(ct<size_t{4}>)};
    const auto part2{solution(ct<size_t{14}>)};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
