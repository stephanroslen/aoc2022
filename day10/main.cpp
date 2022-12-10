#include <aoclib/Coord2D.h>
#include <aoclib/Grid2D.h>

#include <fmt/core.h>

#include <cassert>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

struct Operation {
    enum struct Opcode { Noop, Addx };

    using enum Opcode;

    Opcode opcode;
    int operand;

    friend std::istream& operator>>(std::istream& is, Operation& op)
    {
        std::string opRep{};
        if (is >> opRep) {
            if ("noop" == opRep) {
                op.opcode = Noop;
                op.operand = 0; // not needed, but weird value looks meh in debugger
            } else {
                assert("addx" == opRep);
                op.opcode = Addx;
                is.ignore();
                is >> op.operand;
            }
        }
        return is;
    }
};

using Operations = std::vector<Operation>;

struct CPU {
    int regx{1};
    int cycle{0};

    void run(const Operations& ops, auto&& debugMorphism) noexcept
    {
        using enum Operation::Opcode;

        for (const auto& op : ops) {
            ++cycle;
            debugMorphism(const_cast<const CPU&>(*this));
            switch (op.opcode) {
                case Noop:
                    break;
                case Addx:
                    ++cycle;
                    debugMorphism(const_cast<const CPU&>(*this));
                    regx += op.operand;
                    break;
            }
        }
    }
};

struct Screen {
    using CoordBaseType = int;
    using CoordType = Coord2D<CoordBaseType>;
    using Grid = Grid2D<CoordBaseType, bool>;

    constexpr static CoordType gridSize{40, 6};

    Grid data{gridSize, [](const auto&) noexcept { return false; }};

    void setPixel(const CoordType& coord) noexcept
    {
        assert(data.isOnGrid(coord));
        data.at(coord) = true;
    }

    std::string render() const noexcept
    {
        std::string result{};
        data.eachCoord([&](const auto& coord) {
            if (0 == coord.x && !result.empty()) {
                result += '\n';
            }
            result += data.at(coord) ? '#' : '.';
        });
        return result;
    }
};

int main()
{
    const auto ops{[]() {
        Operations res{};
        std::copy(
            std::istream_iterator<Operation>{std::cin},
            std::istream_iterator<Operation>{},
            std::back_inserter(res));
        return res;
    }()};

    const auto [part1, part2]{[&]() {
        auto result{std::make_tuple(int{0}, std::string{})};
        auto& [resPart1, resPart2]{result};

        CPU cpu{};
        Screen screen{};

        cpu.run(ops, [&](const CPU& c) {
            if (((c.cycle - 20) % 40) == 0) {
                resPart1 += c.cycle * c.regx;
            }
            const Screen::CoordType coord{(c.cycle - 1) % 40, (c.cycle - 1) / 40};
            const auto spriteMid{c.regx};
            if (((spriteMid - 1) <= coord.x) && ((spriteMid + 1) >= coord.x)) {
                screen.setPixel(coord);
            }
        });
        resPart2 = screen.render();

        return result;
    }()};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
