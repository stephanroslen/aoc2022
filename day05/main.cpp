#include <aoclib/CT.h>

#include <fmt/core.h>

#include <deque>
#include <iostream>
#include <tuple>
#include <vector>

enum struct Part { Part1, Part2 };

struct Instruction {
    size_t count;
    size_t from;
    size_t to;

    friend std::istream& operator>>(std::istream& is, Instruction& ins)
    {
        is.ignore(5);
        is >> ins.count;
        is.ignore(6);
        is >> ins.from;
        is.ignore(4);
        is >> ins.to;
        is.ignore();
        return is;
    }
};

struct World {
    std::vector<std::deque<char>> stacks{};

    template <Part part>
    void perform(const Instruction& ins)
    {
        auto& from{stacks[ins.from - 1]};
        auto& to{stacks[ins.to - 1]};

        if constexpr (Part::Part1 == part) {
            for (size_t i{0}; i < ins.count; ++i) {
                const auto c{from.front()};
                from.pop_front();
                to.push_front(c);
            }
        } else {
            const auto fromBegin{std::begin(from)};
            const auto fromTo{[&]() {
                auto res{fromBegin};
                std::advance(res, ins.count);
                return res;
            }()};

            to.insert(std::begin(to), fromBegin, fromTo);
            from.erase(fromBegin, fromTo);
        }
    }

    std::string headline() const
    {
        std::string res{};
        for (const auto& s : stacks) {
            res.push_back(s.front());
        }
        return res;
    }

    friend std::istream& operator>>(std::istream& is, World& w)
    {
        const auto [num, input]{[&]() {
            auto res{std::tuple<size_t, std::vector<std::string>>{}};
            auto& [resNum, resInput]{res};
            std::string tmp{};
            for (;;) {
                std::getline(is, tmp);
                if (tmp.empty()) {
                    break;
                }
                resInput.emplace_back(std::move(tmp));
            }

            const auto last{resInput.back()};
            resInput.pop_back();
            resNum = static_cast<size_t>(last.back() - '0');

            return res;
        }()};

        w.stacks.resize(num);

        for (const auto& line : input) {
            for (size_t i{0}; i < num; ++i) {
                const auto pos{1 + 4 * i};
                if (pos >= line.size()) {
                    break;
                }
                const char c{line[pos]};
                if (' ' != c) {
                    w.stacks[i].push_back(c);
                }
            }
        }

        return is;
    }
};

int main()
{
    const auto world{[]() {
        World res{};
        std::cin >> res;
        return res;
    }()};

    const auto instructions{[]() {
        std::vector<Instruction> res{};
        std::copy(
            std::istream_iterator<Instruction>{std::cin},
            std::istream_iterator<Instruction>{},
            std::back_inserter(res));
        return res;
    }()};

    const auto solution{[&](const auto partSelector) {
        constexpr auto part{decltype(partSelector)::value};
        auto work{world};

        for (const auto& ins : instructions) {
            work.perform<part>(ins);
        }
        return work.headline();
    }};

    const auto part1{solution(ct<Part::Part1>)};
    const auto part2{solution(ct<Part::Part2>)};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
