#include <aoclib/Overloaded.h>

#include <fmt/core.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <variant>
#include <vector>

struct Integer {
    int value{};

    friend std::istream& operator>>(std::istream& is, Integer& integer)
    {
        is >> integer.value;
        return is;
    };

    auto operator<=>(const Integer& other) const noexcept = default;

    bool operator==(const Integer&) const noexcept = default;
};

struct List {
    std::vector<std::variant<Integer, List>> values{};

    List() = default;
    List(const List&) = default;
    List(List&&) = default;
    List& operator=(const List&) = default;
    List& operator=(List&&) = default;

    List(Integer i) noexcept
        : values{std::move(i)} {};

    auto operator<=>(const List& other) const noexcept
    {
        auto iSelf{std::cbegin(values)};
        auto iOther{std::cbegin(other.values)};

        const auto iEndSelf{std::cend(values)};
        const auto iEndOther{std::cend(other.values)};

        while (iSelf != iEndSelf) {
            if (iOther == iEndOther) {
                return std::strong_ordering::greater;
            }

            const auto order{std::visit(
                Overloaded{
                    [&](const Integer& il) {
                        return std::visit(
                            Overloaded{
                                [&](const Integer& ir) { return il <=> ir; },
                                [&](const List& lr) {
                                    const List ll{il};
                                    return ll <=> lr;
                                }},
                            *iOther);
                    },
                    [&](const List& ll) {
                        return std::visit(
                            Overloaded{
                                [&](const Integer& ir) {
                                    const List lr{ir};
                                    return ll <=> lr;
                                },
                                [&](const List& lr) { return ll <=> lr; }},
                            *iOther);
                    }},
                *iSelf)};
            if (std::strong_ordering::equal != order) {
                return order;
            }

            ++iSelf;
            ++iOther;
        }
        if (iOther != iEndOther) {
            return std::strong_ordering::less;
        }
        return std::strong_ordering::equal;
    }

    bool operator==(const List&) const noexcept = default;

    friend std::istream& operator>>(std::istream& is, List& list)
    {
        list.values.clear();
        char v;
        if (is >> v) {
            assert('[' == v);
            for (;;) {
                v = is.peek();
                switch (v) {
                    case '[': {
                        List l{};
                        is >> l;
                        list.values.emplace_back(std::move(l));
                        break;
                    }
                    case ']':
                        is >> v;
                        assert(']' == v);
                        return is;
                    case ',':
                        is >> v;
                        assert(',' == v);
                        break;
                    default:
                        Integer i{};
                        is >> i;
                        list.values.emplace_back(std::move(i));
                }
            }
        }
        return is;
    }
};

struct Packet {
    List value{};

    friend std::istream& operator>>(std::istream& is, Packet& packet)
    {
        is >> packet.value;
        return is;
    };

    auto operator<=>(const Packet&) const noexcept = default;

    bool operator==(const Packet&) const noexcept = default;
};

struct Pair {
    std::array<Packet, 2> packets{};

    friend std::istream& operator>>(std::istream& is, Pair& pair)
    {
        if (is >> pair.packets[0]) {
            is >> pair.packets[1];
        }

        return is;
    };

    bool ordered() const noexcept
    {
        return packets[0].value < packets[1].value;
    }
};

int main()
{
    const auto pairs{[]() {
        std::vector<Pair> result{};
        std::copy(std::istream_iterator<Pair>{std::cin}, std::istream_iterator<Pair>{}, std::back_inserter(result));
        return result;
    }()};

    const auto part1{[&]() {
        size_t result{0};

        for (size_t i{0}; i < pairs.size(); ++i) {
            if (pairs[i].ordered()) {
                result += i + 1;
            }
        }

        return result;
    }()};

    const auto part2{[&]() {
        const Packet div2{List{Integer{2}}};
        const Packet div6{List{Integer{6}}};

        std::vector<Packet> packets{div2, div6};

        for (const Pair& p : pairs) {
            packets.emplace_back(p.packets[0]);
            packets.emplace_back(p.packets[1]);
        }

        std::sort(std::begin(packets), std::end(packets));

        size_t p2{0};
        size_t p6{0};

        for (size_t i{0}; i < packets.size(); ++i) {
            const auto& p{packets[i]};
            if (div2 == p) {
                p2 = i + 1;
            } else if (div6 == p) {
                p6 = i + 1;
                break;
            }
        }

        return p2 * p6;
    }()};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
