#include <aoclib/CT.h>
#include <aoclib/Coord2D.h>

#include <fmt/core.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <vector>

enum struct Part { Part1, Part2 };

using CoordBaseType = long int;
using CoordType = Coord2D<CoordBaseType>;

struct Range {
    CoordBaseType from{};
    CoordBaseType to{};

    bool overlapsOrTouches(const Range& other) const noexcept
    {
        assert(from <= other.from);
        return to >= other.from - 1;
    }

    void merge(const Range& other) noexcept
    {
        assert(overlapsOrTouches(other));
        if (to >= other.to) {
            return;
        }
        to = other.to;
    }

    CoordBaseType width() const noexcept
    {
        return to - from + 1;
    }
};

struct Sensor {
    CoordType sensor{};
    CoordType beacon{};

    CoordBaseType dist() const noexcept
    {
        const auto diff{beacon - sensor};
        return std::abs(diff.x) + std::abs(diff.y);
    }

    template <Part part>
    std::optional<Range> rangeAtY(const CoordBaseType loc) const noexcept
    {
        const auto yDist{std::abs(sensor.y - loc)};
        const auto mDist{dist()};

        if (yDist > mDist) {
            return std::nullopt;
        }

        if constexpr (part == Part::Part1) {
            if (yDist == mDist && sensor.y == loc) {
                return std::nullopt;
            }
        }

        const auto residual{mDist - yDist};

        Range result{sensor.x - residual, sensor.x + residual};
        if constexpr (part == Part::Part1) {
            if (beacon.y == loc) {
                if (result.from == beacon.x) {
                    result.from += 1;
                } else {
                    assert(result.to == beacon.x);
                    result.to -= 1;
                }
            }
        }
        return result;
    }

    friend std::istream& operator>>(std::istream& is, Sensor& s)
    {
        if (is.good()) {
            is.ignore(12);
            is >> s.sensor.x;
            is.ignore(4);
            is >> s.sensor.y;
            is.ignore(25);
            is >> s.beacon.x;
            is.ignore(4);
            is >> s.beacon.y;
            if (is.good()) {
                is.ignore(1);
            }
        }
        return is;
    }
};

int main()
{
    const auto sensors{[]() {
        std::vector<Sensor> result{};
        std::copy(std::istream_iterator<Sensor>{std::cin}, std::istream_iterator<Sensor>{}, std::back_inserter(result));
        return result;
    }()};

    static constexpr CoordBaseType loc1{2000000};
    static constexpr CoordBaseType maxLoc{4000000};

    const auto rangesAt{[&]<Part part>(const CT<part>, const CoordBaseType loc) {
        std::vector<Range> result{};

        for (const auto& s : sensors) {
            const auto rOpt{s.rangeAtY<part>(loc)};
            if (rOpt) {
                result.emplace_back(*rOpt);
            }
        }

        std::sort(std::begin(result), std::end(result), [](const Range& a, const Range& b) { return a.from < b.from; });

        for (size_t i{0}; i + 1 < result.size();) {
            if (result.at(i).overlapsOrTouches(result.at(i + 1))) {
                result.at(i).merge(result.at(i + 1));
                result.erase(std::cbegin(result) + i + 1);
            } else {
                ++i;
            }
        }

        return result;
    }};

    const auto part1{[&]() {
        CoordBaseType result{0};
        for (const auto& r : rangesAt(ct<Part::Part1>, loc1)) {
            result += r.width();
        }
        return result;
    }()};

    const auto part2{[&]() {
        for (CoordBaseType iLoc{0}; iLoc <= maxLoc; ++iLoc) {
            const auto r{rangesAt(ct<Part::Part2>, iLoc)};
            if (r.size() == 2) {
                return maxLoc * (r.at(0).to + 1) + iLoc;
            }
        }
        std::terminate();
    }()};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
