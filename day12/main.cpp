#include <aoclib/Coord2D.h>
#include <aoclib/Grid2D.h>

#include <fmt/core.h>

#include <cassert>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

struct World {
    using BaseCoordType = int;
    using CoordType = Coord2D<BaseCoordType>;

    using GridType = detail::Grid2D<BaseCoordType, int>;

    GridType map{};
    CoordType from{-1, -1};
    CoordType to{-1, -1};

    friend std::istream& operator>>(std::istream& is, World& world) noexcept
    {
        std::vector<std::string> lines{};
        std::string tmp{};
        while (std::getline(is, tmp)) {
            lines.emplace_back(std::move(tmp));
        }

        const CoordType worldSize{static_cast<int>(lines.front().size()), static_cast<int>(lines.size())};

        world.map = GridType{worldSize, [&](const CoordType& coord) {
                                 const auto val{lines[coord.y][coord.x]};
                                 switch (val) {
                                     case 'S':
                                         world.from = coord;
                                         return 0;
                                     case 'E':
                                         world.to = coord;
                                         return 25;
                                     default:
                                         return static_cast<int>(lines[coord.y][coord.x] - 'a');
                                 }
                             }};

        assert(world.from != (CoordType{-1, -1}));
        assert(world.to != (CoordType{-1, -1}));

        return is;
    }

    GridType distMap() const noexcept
    {
        GridType result{map.gridSize, [&](const CoordType& coord) {
                            return coord == to ? 0 : std::numeric_limits<int>::max();
                        }};

        std::queue<CoordType> fringe{};

        const auto handle{[&](const CoordType& loc) {
            const auto curVal{result.at(loc)};
            for (const auto& offset : CoordType::adjacencies()) {
                const auto candidate{loc + offset};
                if (map.isOnGrid(candidate) && map.at(loc) <= map.at(candidate) + 1
                    && result.at(candidate) > curVal + 1)
                {
                    result.at(candidate) = curVal + 1;
                    fringe.push(candidate);
                }
            }
        }};

        handle(to);

        while (!fringe.empty()) {
            const auto loc{fringe.front()};
            fringe.pop();
            handle(loc);
        }

        return result;
    }
};

int main()
{
    const auto world{[]() {
        World result{};
        std::cin >> result;
        return result;
    }()};

    const auto dm{world.distMap()};

    const auto part1{dm.at(world.from)};

    const auto part2{[&]() {
        int result{std::numeric_limits<int>::max()};
        world.map.eachCoord([&](const World::CoordType& coord) {
            if (0 == world.map.at(coord)) {
                result = std::min(result, dm.at(coord));
            }
        });
        return result;
    }()};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
