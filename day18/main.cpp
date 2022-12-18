#include <aoclib/Coord3D.h>

#include <fmt/core.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <vector>

using CoordBaseType = int;
using CoordType = Coord3D<CoordBaseType>;

struct Droplet {
    CoordType loc;

    friend std::istream& operator>>(std::istream& is, Droplet& droplet)
    {
        if (is >> droplet.loc.x) {
            is.ignore(1);
            is >> droplet.loc.y;
            is.ignore(1);
            is >> droplet.loc.z;
        }
        return is;
    }
};

int main()
{
    const auto droplets{[]() {
        std::vector<Droplet> result{};
        std::copy(
            std::istream_iterator<Droplet>{std::cin},
            std::istream_iterator<Droplet>{},
            std::back_inserter(result));
        return result;
    }()};

    const auto [boundMin, boundMax, dropLocHash]{[&]() {
        auto result = std::make_tuple(
            CoordType{
                std::numeric_limits<CoordBaseType>::max(),
                std::numeric_limits<CoordBaseType>::max(),
                std::numeric_limits<CoordBaseType>::max()},
            CoordType{
                std::numeric_limits<CoordBaseType>::min(),
                std::numeric_limits<CoordBaseType>::min(),
                std::numeric_limits<CoordBaseType>::min()},
            std::unordered_set<CoordType, CoordType::Hasher>{});
        auto& [resBoundMin, resBoundMax, resDropLocHash]{result};

        for (const auto& d : droplets) {
            resBoundMin.x = std::min(d.loc.x, resBoundMin.x);
            resBoundMin.y = std::min(d.loc.y, resBoundMin.y);
            resBoundMin.z = std::min(d.loc.z, resBoundMin.z);

            resBoundMax.x = std::max(d.loc.x, resBoundMax.x);
            resBoundMax.y = std::max(d.loc.y, resBoundMax.y);
            resBoundMax.z = std::max(d.loc.z, resBoundMax.z);

            resDropLocHash.emplace(d.loc);
        }

        return result;
    }()};

    const auto solution{[&](auto&& conditionMorphism) {
        size_t result{0};
        for (const auto& d : droplets) {
            for (const auto& adj : CoordType::adjacencies()) {
                if (conditionMorphism(d.loc + adj)) {
                    ++result;
                }
            }
        }
        return result;
    }};

    const auto part1{solution([&](const CoordType& l) { return !dropLocHash.contains(l); })};

    const auto isOutside{[&](const CoordType& l) {
        static std::unordered_set<CoordType, CoordType::Hasher> insideCache{};
        static std::unordered_set<CoordType, CoordType::Hasher> outsideCache{};

        std::queue<CoordType> fringe{};
        std::unordered_set<CoordType, CoordType::Hasher> visited{};

        if (!dropLocHash.contains(l)) {
            visited.emplace(l);
            fringe.emplace(l);
        }

        while (!fringe.empty()) {
            const CoordType val{fringe.front()};
            fringe.pop();

            if (outsideCache.contains(val) || !val.isInCuboid(boundMin, boundMax)) {
                outsideCache.merge(visited);
                return true;
            }

            if (insideCache.contains(val)) {
                break;
            }

            for (const auto& adj : CoordType::adjacencies()) {
                const auto cand{val + adj};
                if (!visited.contains(cand) && !dropLocHash.contains(cand)) {
                    visited.emplace(cand);
                    fringe.emplace(cand);
                }
            }
        }

        insideCache.merge(visited);
        return false;
    }};

    const auto part2{solution([&](const CoordType& l) { return isOutside(l); })};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
