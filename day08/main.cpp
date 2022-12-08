#include <aoclib/Coord2D.h>
#include <aoclib/Grid2D.h>

#include <fmt/core.h>

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct World {
    using BaseCoordType = int;
    using CoordType = Coord2D<BaseCoordType>;

    using GridType = detail::Grid2D<BaseCoordType, int>;

    GridType map{};

    friend std::istream& operator>>(std::istream& is, World& world) noexcept
    {
        std::vector<std::string> lines{};
        std::string tmp{};
        while (std::getline(is, tmp)) {
            lines.emplace_back(std::move(tmp));
        }

        const CoordType worldSize{static_cast<int>(lines.front().size()), static_cast<int>(lines.size())};

        world.map = GridType{worldSize, [&](const CoordType& coord) {
                                 return static_cast<int>(lines[coord.y][coord.x] - '0');
                             }};

        return is;
    }

    size_t visibleTrees() const noexcept
    {
        size_t result{0};
        map.eachCoord([&](const CoordType& coord) {
            const auto height{map.at(coord)};
            for (const auto& dir : CoordType::adjacencies()) {
                for (auto iterCoord{coord + dir}; map.isOnGrid(iterCoord); iterCoord += dir) {
                    const auto iterCoordHeight{map.at(iterCoord)};
                    if (iterCoordHeight >= height) {
                        goto nextDir;
                    }
                }
                result += 1;
                return;
            nextDir:;
            }
        });
        return result;
    }

    size_t treesVisibleFromInDirection(const CoordType& from, const CoordType& dir) const noexcept
    {
        size_t result{0};
        const auto height{map.at(from)};
        for (auto iterCoord{from + dir}; map.isOnGrid(iterCoord); iterCoord += dir) {
            result += 1;
            if (map.at(iterCoord) >= height) {
                break;
            }
        }
        return result;
    }

    size_t scenicScore(const CoordType& from) const noexcept
    {
        constexpr auto adj{CoordType::adjacencies()};
        return std::transform_reduce(
            std::cbegin(adj),
            std::cend(adj),
            size_t{1},
            std::multiplies{},
            [&](const auto& dir) { return treesVisibleFromInDirection(from, dir); });
    }

    size_t highestScenicScore() const noexcept
    {
        size_t result{0};
        map.eachCoord([&](const CoordType& coord) { result = std::max(result, scenicScore(coord)); });
        return result;
    }
};

int main()
{
    const auto world{[]() {
        World res{};
        std::cin >> res;
        return res;
    }()};

    fmt::print("{}\n", world.visibleTrees());
    fmt::print("{}\n", world.highestScenicScore());
}
