#include <aoclib/CT.h>
#include <aoclib/Coord2D.h>

#include <fmt/core.h>

#include <array>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

using BaseCoordType = int;
using CoordType = Coord2D<int>;

enum struct Direction { Up, Right, Down, Left };

struct Movement {
    Direction dir;
    size_t count;

    constexpr static std::array<CoordType, 4> directionOffset{{{0, -1}, {1, 0}, {0, 1}, {-1, 0}}};

    constexpr CoordType getOffset() const noexcept
    {
        return directionOffset[static_cast<size_t>(dir)];
    }

    friend std::istream& operator>>(std::istream& is, Movement& m)
    {
        char tmp;
        if (is >> tmp) {
            switch (tmp) {
                case 'U':
                    m.dir = Direction::Up;
                    break;
                case 'R':
                    m.dir = Direction::Right;
                    break;
                case 'D':
                    m.dir = Direction::Down;
                    break;
                case 'L':
                    m.dir = Direction::Left;
                    break;
                default:
                    std::terminate();
            }
            is.ignore();
            is >> m.count;
        }
        return is;
    }
};

using Movements = std::vector<Movement>;

template <size_t knots>
struct Simulation {
    static auto initKnotCoords()
    {
        std::array<CoordType, knots> res{};
        std::fill(std::begin(res), std::end(res), CoordType{0, 0});
        return res;
    };

    std::array<CoordType, knots> knotCoords{initKnotCoords()};

    std::unordered_set<CoordType, CoordType::Hasher> tailVisited{knotCoords[knots - 1]};

    size_t tailVisitedCount() const noexcept
    {
        return tailVisited.size();
    }

    void fixKnots() noexcept
    {
        for (size_t preKnotNum{0}; preKnotNum < knots - 1; ++preKnotNum) {
            const size_t succKnotNum{preKnotNum + 1};
            const auto& posPre{knotCoords[preKnotNum]};
            auto& posSucc{knotCoords[succKnotNum]};

            const auto fixVecOpt{[&]() -> std::optional<CoordType> {
                auto res{posPre - posSucc};

                bool doNotFix{true};

                const auto fixCoord([&](auto& coord) {
                    if (coord == 2) {
                        doNotFix = false;
                        coord = 1;
                    } else if (coord == -2) {
                        doNotFix = false;
                        coord = -1;
                    }
                });

                fixCoord(res.x);
                fixCoord(res.y);

                if (doNotFix) {
                    return std::nullopt;
                }
                return res;
            }()};

            posSucc += fixVecOpt.value_or(CoordType{0, 0});
        }
        tailVisited.emplace(knotCoords[knots - 1]);
    }

    void step(const Movement& m) noexcept
    {
        knotCoords[0] += m.getOffset();
        fixKnots();
    }

    void simulate(const Movements& ms) noexcept
    {
        for (const auto& m : ms) {
            for (size_t i{0}; i < m.count; ++i) {
                step(m);
            }
        }
    }
};

int main()
{
    const auto input{[]() {
        Movements res{};
        std::copy(std::istream_iterator<Movement>{std::cin}, std::istream_iterator<Movement>{}, std::back_inserter(res));
        return res;
    }()};

    const auto solution{[&](const auto ctKnots) {
        constexpr auto knots{decltype(ctKnots)::value};
        Simulation<knots> sim{};
        sim.simulate(input);
        return sim.tailVisitedCount();
    }};

    fmt::print("{}\n", solution(ct<size_t{2}>));
    fmt::print("{}\n", solution(ct<size_t{10}>));
}
