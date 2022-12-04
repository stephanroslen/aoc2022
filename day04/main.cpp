#include <aoclib/CT.h>

#include <fmt/core.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

struct Assignment {
    int from;
    int to;

    friend std::istream& operator>>(std::istream& is, Assignment& a)
    {
        is >> a.from;
        is.ignore();
        is >> a.to;
        return is;
    }

    constexpr bool contains(const Assignment& other) const noexcept
    {
        return (from <= other.from) && (to >= other.to);
    }

    constexpr bool startsIn(const Assignment& other) const noexcept
    {
        return (from >= other.from) && (from <= other.to);
    }

    constexpr bool stopsIn(const Assignment& other) const noexcept
    {
        return (to >= other.from) && (to <= other.to);
    }

    constexpr bool startsOrStopsIn(const Assignment& other) const noexcept
    {
        return startsIn(other) || stopsIn(other);
    }
};

struct AssignemntPair {
    std::array<Assignment, 2> assignments;

    friend std::istream& operator>>(std::istream& is, AssignemntPair& a)
    {
        is >> a.assignments[0];
        is.ignore();
        is >> a.assignments[1];
        return is;
    }

    constexpr bool oneContainsOther() const noexcept
    {
        return assignments[0].contains(assignments[1]) || assignments[1].contains(assignments[0]);
    }

    constexpr bool oneStartsOrStopsInOther() const noexcept
    {
        return assignments[0].startsOrStopsIn(assignments[1]) || assignments[1].startsOrStopsIn(assignments[0]);
    }
};

int main()
{
    const auto assignments{[]() {
        std::vector<AssignemntPair> res{};
        std::copy(
            std::istream_iterator<AssignemntPair>{std::cin},
            std::istream_iterator<AssignemntPair>{},
            std::back_inserter(res));
        return res;
    }()};

    const auto solution{[&](const auto methodSelector) {
        return std::count_if(std::cbegin(assignments), std::cend(assignments), [&](const auto& ass) {
            constexpr auto memFun{decltype(methodSelector)::value};
            return (ass.*memFun)();
        });
    }};

    const auto part1{solution(ct<&AssignemntPair::oneContainsOther>)};
    const auto part2{solution(ct<&AssignemntPair::oneStartsOrStopsInOther>)};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
