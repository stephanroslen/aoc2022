#include <fmt/core.h>

#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Rucksack {
    std::string content{};

    friend std::istream& operator>>(std::istream& is, Rucksack& r)
    {
        std::getline(is, r.content);
        return is;
    }

    std::array<std::string_view, 2> comps() const noexcept
    {
        const auto halfsize{content.size() / 2};
        std::array<std::string_view, 2> result{content, content};
        result[0].remove_suffix(halfsize);
        result[1].remove_prefix(halfsize);
        return result;
    }

    char common() const noexcept
    {
        const auto [comp1, comp2]{comps()};
        for (const auto& c : comp1) {
            if (comp2.find(c) != std::string_view::npos) {
                return c;
            }
        }
        std::terminate();
    }

    char badge(const Rucksack& r1, const Rucksack& r2) const noexcept
    {
        for (const auto& c : content) {
            if (r1.content.find(c) != std::string::npos && r2.content.find(c) != std::string::npos) {
                return c;
            }
        }
        std::terminate();
    }

    static size_t prio(const char c) noexcept
    {
        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 1;
        }
        return c - 'A' + 27;
    }
};

int main()
{
    const auto rucksacks{[]() {
        std::vector<Rucksack> res{};
        std::copy(std::istream_iterator<Rucksack>{std::cin}, std::istream_iterator<Rucksack>{}, std::back_inserter(res));
        return res;
    }()};

    const size_t part1{
        std::transform_reduce(std::cbegin(rucksacks), std::cend(rucksacks), size_t{0}, std::plus{}, [](const auto& r) {
            return Rucksack::prio(r.common());
        })};
    const auto part2{[&]() {
        std::size_t res{0};

        auto iter{std::cbegin(rucksacks)};
        while (iter != std::cend(rucksacks)) {
            const auto& r0{*iter++};
            assert(iter != std::cend(rucksacks));
            const auto& r1{*iter++};
            assert(iter != std::cend(rucksacks));
            const auto& r2{*iter++};

            res += Rucksack::prio(r0.badge(r1, r2));
        }

        return res;
    }()};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
