#include <aoclib/Coord2D.h>

#include <fmt/core.h>

#include <bits/stdc++.h>

using CoordBaseType = int;
using CoordType = Coord2D<CoordBaseType>;

struct World {
    std::unordered_set<CoordType, CoordType::Hasher> elfLocs{};

    friend std::istream& operator>>(std::istream& is, World& world)
    {
        world.elfLocs.clear();
        std::string line;
        CoordBaseType y{0};
        while (std::getline(is, line)) {
            for (int x{0}; x < static_cast<int>(line.size()); ++x) {
                if (line[x] == '#') {
                    world.elfLocs.emplace(CoordType{x, y});
                }
            }
            ++y;
        }
        return is;
    }

    bool noMoveConsidered(const CoordType& coord) const noexcept
    {
        for (const auto& adj : CoordType::adjacenciesDiag()) {
            if (elfLocs.contains(coord + adj)) {
                return false;
            }
        }
        return true;
    }

    bool check(const CoordType& to, const CoordType& check0, const CoordType& check1) const noexcept
    {
        return (!elfLocs.contains(to) && !elfLocs.contains(check0) && !elfLocs.contains(check1));
    }

    bool simulate(const size_t step) noexcept
    {
        struct Consideration {
            CoordType from;
            CoordType to;
        };
        std::unordered_map<CoordType, size_t, CoordType::Hasher> numConsiderations{};
        std::vector<Consideration> considerations{};

        const auto consider{[&](const auto& from, const auto& to, const auto& check0, const auto& check1) {
            if (check(to, check0, check1)) {
                considerations.emplace_back(Consideration{from, to});
                ++numConsiderations[to];
                return true;
            }
            return false;
        }};

        struct Offsets {
            CoordType actual;
            CoordType check0;
            CoordType check1;
        };

        const std::array<Offsets, 4> offsets{
            {{{0, -1}, {-1, -1}, {1, -1}},
             {{0, 1}, {-1, 1}, {1, 1}},
             {{-1, 0}, {-1, 1}, {-1, -1}},
             {{1, 0}, {1, 1}, {1, -1}}}};

        for (const auto& loc : elfLocs) {
            if (!noMoveConsidered(loc)) {
                for (size_t i{0}; i < 4; ++i) {
                    const size_t actual{(i + step) % 4};
                    const auto& off{offsets[actual]};
                    if (consider(loc, loc + off.actual, loc + off.check0, loc + off.check1)) {
                        break;
                    }
                }
            }
        }

        for (const auto& [from, to] : considerations) {
            if (numConsiderations.at(to) == 1) {
                elfLocs.erase(from);
                elfLocs.emplace(to);
            }
        }

        return !considerations.empty();
    }

    auto bounds() const noexcept
    {
        auto result{std::make_pair(CoordType{0, 0}, CoordType{0, 0})};
        auto& [tl, br]{result};

        for (const auto& loc : elfLocs) {
            tl.x = std::min(tl.x, loc.x);
            tl.y = std::min(tl.y, loc.y);
            br.x = std::max(br.x, loc.x + 1);
            br.y = std::max(br.y, loc.y + 1);
        }

        return result;
    }

    size_t emptyGrounds() const noexcept
    {
        size_t result{0};
        const auto [tl, br]{bounds()};
        for (int iy{tl.y}; iy < br.y; ++iy) {
            for (int ix{tl.x}; ix < br.x; ++ix) {
                if (!elfLocs.contains({ix, iy})) {
                    ++result;
                }
            }
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const World& world)
    {
        const auto [tl, br]{world.bounds()};
        for (int iy{tl.y}; iy < br.y; ++iy) {
            for (int ix{tl.x}; ix < br.x; ++ix) {
                if (world.elfLocs.contains({ix, iy})) {
                    os << '#';
                } else {
                    os << '.';
                }
            }
            os << '\n';
        }
        return os;
    }
};

int main()
{
    World world{};
    std::cin >> world;

    size_t part1{0};
    size_t part2{0};

    for (size_t i{0};; ++i) {
        const bool cont{world.simulate(i)};
        if (i == 9) {
            part1 = world.emptyGrounds();
        }
        if (!cont) {
            part2 = i + 1;
            break;
        }
    }

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
