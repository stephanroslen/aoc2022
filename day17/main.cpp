#include <aoclib/Coord2D.h>

#include <fmt/core.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using CoordBaseType = int;
using CoordType = Coord2D<CoordBaseType>;

struct Movement {
    enum struct Direction { Left, Right };
    Direction direction;

    friend std::istream& operator>>(std::istream& is, Movement& m)
    {
        char c;
        if (is >> c) {
            if (c == '<') {
                m.direction = Direction::Left;
            } else {
                assert(c == '>');
                m.direction = Direction::Right;
            }
        }
        return is;
    }

    CoordType offset() const noexcept
    {
        switch (direction) {
            case Direction::Left:
                return {-1, 0};
            case Direction::Right:
            default:
                return {1, 0};
        }
    }
};

struct Block {
    std::vector<CoordType> offsets;
    CoordBaseType width;
    CoordBaseType height;

    Block(const std::initializer_list<CoordType> coords)
        : offsets{coords}
        , width{0}
        , height{0}
    {
        for (const auto& o : offsets) {
            width = std::max(width, o.x + 1);
            height = std::max(height, o.y + 1);
        }
    }

    Block(const Block&) = default;
    Block(Block&&) = default;
    Block& operator=(const Block&) = default;
    Block& operator=(Block&&) = default;
};

static const Block block0{{0, 0}, {1, 0}, {2, 0}, {3, 0}};
static const Block block1{{0, 1}, {1, 1}, {2, 1}, {1, 0}, {1, 2}};
static const Block block2{{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}};
static const Block block3{{0, 0}, {0, 1}, {0, 2}, {0, 3}};
static const Block block4{{0, 0}, {1, 0}, {0, 1}, {1, 1}};

static const std::vector<Block> blocks{block0, block1, block2, block3, block4};

struct World {
    std::unordered_set<CoordType, CoordType::Hasher> blocked{};
    CoordBaseType maxY{0};

    CoordType spawnLoc() const noexcept
    {
        return {3, maxY + 4};
    }

    bool collides(const CoordType& offset, const Block& block) const noexcept
    {
        return std::any_of(std::cbegin(block.offsets), std::cend(block.offsets), [&](const CoordType& coord) {
            return blocked.contains(coord + offset);
        });
    }

    void block(const CoordType& offset, const Block& block) noexcept
    {
        for (const auto& o : block.offsets) {
            const auto l{offset + o};
            assert(!blocked.contains(l));
            blocked.emplace(l);
        }
        maxY = std::max(maxY, offset.y + block.height - 1);
    }
};

struct CacheState {
    size_t blockIdx;
    size_t movIdx;

    bool operator==(const CacheState&) const noexcept = default;

    struct Hasher {
        size_t operator()(const CacheState& cs) const
        {
            return cs.blockIdx ^ (cs.movIdx << 32) ^ (cs.movIdx >> 32);
        }
    };
};

struct CacheValues {
    CoordBaseType maxY;
    size_t blockNum;
};

struct Period {
    CoordBaseType y;
    size_t blocks;
};

int main()
{
    const auto movements{[]() {
        std::vector<Movement> result{};
        std::copy(
            std::istream_iterator<Movement>{std::cin},
            std::istream_iterator<Movement>{},
            std::back_inserter(result));
        return result;
    }()};

    constexpr size_t startCachingAt{10000};

    const auto solution{[&](const size_t blockCount) {
        World world{};

        std::unordered_map<CacheState, CacheValues, CacheState::Hasher> cache{};

        size_t blockIdx{0};
        size_t movIdx{0};

        std::optional<Period> period{};

        size_t goal{blockCount};

        for (size_t i{0}; i < goal; ++i) {
            if (i >= startCachingAt && !period) {
                const CacheState state{blockIdx, movIdx};
                if (const auto it{cache.find(state)}; std::cend(cache) != it) {
                    period = {world.maxY - it->second.maxY, i - it->second.blockNum};
                    const auto missingBlocks{blockCount - i};
                    const auto gap{missingBlocks % period->blocks};
                    goal = i + gap;
                }
                cache.emplace(state, CacheValues{world.maxY, i});
            }

            const auto& block{blocks.at(blockIdx)};

            auto loc{world.spawnLoc()};
            for (;;) {
                const auto& movement{movements.at(movIdx)};

                movIdx = (movIdx + 1) % movements.size();

                const auto tryLoc{loc + movement.offset()};
                if (tryLoc.x >= 1 && tryLoc.x + block.width - 1 <= 7 && !world.collides(tryLoc, block)) {
                    loc = tryLoc;
                }
                const auto fallLoc{loc + CoordType{0, -1}};
                if (fallLoc.y == 0 || world.collides(fallLoc, block)) {
                    break;
                }
                loc = fallLoc;
            }
            world.block(loc, block);

            blockIdx = (blockIdx + 1) % blocks.size();
        }

        auto result{static_cast<size_t>(world.maxY)};
        if (period) {
            result += (blockCount - goal) / period->blocks * static_cast<size_t>(period->y);
        }
        return result;
    }};

    fmt::print("{}\n", solution(2022));
    fmt::print("{}\n", solution(1000000000000));
}
