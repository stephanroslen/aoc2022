#include <aoclib/CT.h>

#include <fmt/core.h>

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

enum class Mode { Part1, Part2 };

struct Step {
    int other;
    int meOrOutcome;

    friend std::istream& operator>>(std::istream& is, Step& s)
    {
        std::string line{};
        std::getline(is, line);
        s.other = static_cast<int>(line[0] - 'A');
        s.meOrOutcome = static_cast<int>(line[2] - 'X');
        return is;
    }

    template <Mode mode>
    constexpr int pick() const noexcept
    {
        if constexpr (mode == Mode::Part1) {
            return meOrOutcome;
        } else {
            return (other - 1 + meOrOutcome + 3) % 3;
        }
    }

    template <Mode mode>
    constexpr int outcome() const noexcept
    {
        if constexpr (mode == Mode::Part2) {
            return meOrOutcome;
        } else {
            if (meOrOutcome == other) {
                return 1;
            }
            if ((meOrOutcome == other + 1) || (meOrOutcome == other - 2)) {
                return 2;
            }
            return 0;
        }
    }

    template <Mode mode>
    constexpr size_t score() const noexcept
    {
        return pick<mode>() + 1 + outcome<mode>() * 3;
    }
};

int main()
{
    const auto game{[]() {
        std::vector<Step> res{};
        std::copy(std::istream_iterator<Step>{std::cin}, std::istream_iterator<Step>{}, std::back_inserter(res));
        return res;
    }()};

    const auto task{[&](const auto MT) {
        return std::transform_reduce(std::cbegin(game), std::cend(game), size_t{0}, std::plus{}, [](const auto val) {
            return val.template score<decltype(MT)::value>();
        });
    }};

    fmt::print("{}\n", task(ct<Mode::Part1>));
    fmt::print("{}\n", task(ct<Mode::Part2>));
}
