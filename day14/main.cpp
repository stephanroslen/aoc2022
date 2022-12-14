#include <aoclib/CT.h>
#include <aoclib/Coord2D.h>

#include <fmt/core.h>

#include <iostream>
#include <optional>
#include <unordered_set>
#include <vector>

using BaseCoordType = int;
using CoordType = Coord2D<BaseCoordType>;

enum struct Part { Part1, Part2 };

struct Line {
    std::vector<CoordType> coords{};

    void eachCoord(auto&& morphism) const noexcept
    {
        auto it0{std::cbegin(coords)};
        auto it1{it0};
        ++it1;
        while (std::cend(coords) != it1) {
            const auto from{*it0};
            const auto to{*it1};

            const auto diff{to - from};
            auto direction{diff};
            if (direction.x < 0) {
                direction.x = -1;
            } else if (direction.x > 0) {
                direction.x = 1;
            }
            if (direction.y < 0) {
                direction.y = -1;
            } else if (direction.y > 0) {
                direction.y = 1;
            }

            for (CoordType walk{from}; walk != to; walk += direction) {
                morphism(walk);
            }

            ++it0;
            ++it1;
        }
        morphism(*it0);
    }

    friend std::istream& operator>>(std::istream& is, Line& line)
    {
        line.coords.clear();

        CoordType coord{};
        char tmpChar{};
        while (is >> coord.x) {
            is.ignore();
            is >> coord.y;
            line.coords.emplace_back(std::move(coord));
            if (!is.good()) {
                break;
            }
            tmpChar = is.peek();
            if (' ' == tmpChar) {
                is.ignore(4);
            } else if ('\n' == tmpChar) {
                break;
            }
        }
        return is;
    }
};

struct Lines {
    std::vector<Line> lines{};

    friend std::istream& operator>>(std::istream& is, Lines& l)
    {
        l.lines.clear();

        std::copy(std::istream_iterator<Line>{is}, std::istream_iterator<Line>{}, std::back_inserter(l.lines));
        return is;
    }

    void eachCoord(auto&& morphism) const noexcept
    {
        for (const auto& l : lines) {
            l.eachCoord(std::forward<decltype(morphism)>(morphism));
        }
    }
};

struct Map {
    std::unordered_set<CoordType, CoordType::Hasher> blocked{};

    CoordType minCoord{0, 0};
    CoordType maxCoord{0, 0};

    void block(const CoordType& coord) noexcept
    {
        blocked.emplace(coord);
    }

    void blockAndUpdate(const CoordType& coord) noexcept
    {
        block(coord);

        minCoord.x = std::min(minCoord.x, coord.x);
        minCoord.y = std::min(minCoord.y, coord.y);
        maxCoord.x = std::max(maxCoord.x, coord.x);
        maxCoord.y = std::max(maxCoord.y, coord.y);
    }

    template <Part part>
    bool isBlocked(const CoordType& coord) const noexcept
    {
        if constexpr (Part::Part2 == part) {
            if (coord.y == maxCoord.y + 2) {
                return true;
            }
        } else {
            static_assert(Part::Part1 == part);
        }

        return blocked.contains(coord);
    }

    template <Part part>
    std::conditional_t<part == Part::Part1, std::optional<CoordType>, CoordType> simulateDrop(const CoordType& from)
    {
        if constexpr (Part::Part1 == part) {
            if (from.x < minCoord.x || from.y < minCoord.y || from.x > maxCoord.x || from.y > maxCoord.y) {
                return std::nullopt;
            }
        } else {
            static_assert(Part::Part2 == part);
        }

        for (const auto& offset : {CoordType{0, 1}, CoordType{-1, 1}, CoordType{1, 1}}) {
            const auto candidate{from + offset};
            if (!isBlocked<part>(candidate)) {
                return simulateDrop<part>(candidate);
            }
        }

        return from;
    }
};

int main()
{
    const auto lines{[]() {
        Lines result{};
        std::cin >> result;
        return result;
    }()};

    const auto map{[&]() {
        Map result{};
        lines.eachCoord([&](const CoordType& coord) { result.blockAndUpdate(coord); });
        return result;
    }()};

    constexpr CoordType source{500, 0};

    const auto solution{[&](const auto ctPart) {
        constexpr auto part{decltype(ctPart)::value};
        size_t result{0};
        auto workMap{map};

        for (;;) {
            const auto target{workMap.simulateDrop<part>(source)};

            if constexpr (Part::Part1 == part) {
                if (!target) {
                    break;
                }
                result += 1;
                workMap.block(*target);
            } else {
                static_assert(Part::Part2 == part);
                result += 1;
                if (source == target) {
                    break;
                }
                workMap.block(target);
            }
        }

        return result;
    }};

    const auto part1{solution(ct<Part::Part1>)};
    const auto part2{solution(ct<Part::Part2>)};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
