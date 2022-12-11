#include <fmt/core.h>

#include <array>
#include <deque>
#include <functional>
#include <vector>

struct Monkey {
    using ItemType = long int;

    using ConvertedItemType = std::vector<int>;

    std::deque<ItemType> items{};
    std::function<ItemType(ItemType)> operation{};
    int divCheck{};
    std::array<size_t, 2> targets{};

    std::deque<ConvertedItemType> convertedItems{};

    size_t inspectionCnt{0};
};

int main()
{
#if 1
    std::vector<Monkey> monkeys{
        Monkey{{54, 98, 50, 94, 69, 62, 53, 85}, [](const auto old) { return old * 13; }, 3, {{2, 1}}},
        Monkey{{71, 55, 82}, [](const auto old) { return old + 2; }, 13, {{7, 2}}},
        Monkey{{77, 73, 86, 72, 87}, [](const auto old) { return old + 8; }, 19, {{4, 7}}},
        Monkey{{97, 91}, [](const auto old) { return old + 1; }, 17, {{6, 5}}},
        Monkey{{78, 97, 51, 85, 66, 63, 62}, [](const auto old) { return old * 17; }, 5, {{6, 3}}},
        Monkey{{88}, [](const auto old) { return old + 3; }, 7, {{1, 0}}},
        Monkey{{87, 57, 63, 86, 87, 53}, [](const auto old) { return old * old; }, 11, {{5, 0}}},
        Monkey{{73, 59, 82, 65}, [](const auto old) { return old + 6; }, 2, {{4, 3}}}};
#else
    std::vector<Monkey> monkeys{
        Monkey{{79, 98}, [](const auto old) { return old * 19; }, 23, {{2, 3}}},
        Monkey{{54, 65, 75, 74}, [](const auto old) { return old + 6; }, 19, {{2, 0}}},
        Monkey{{79, 60, 97}, [](const auto old) { return old * old; }, 13, {{1, 3}}},
        Monkey{{74}, [](const auto old) { return old + 3; }, 17, {{0, 1}}}};
#endif

    auto work1{monkeys};
    for (size_t i{0}; i < 20; ++i) {
        for (auto& monkey : work1) {
            while (!monkey.items.empty()) {
                monkey.inspectionCnt += 1;

                auto item = monkey.items.front();
                monkey.items.pop_front();

                item = monkey.operation(item);
                item /= 3;

                const auto target = monkey.targets[((item % monkey.divCheck) == 0) ? 0 : 1];
                work1[target].items.push_back(item);
            }
        }
    }

    std::sort(std::begin(work1), std::end(work1), [](const auto& m1, const auto& m2) {
        return m1.inspectionCnt > m2.inspectionCnt;
    });

    for (auto& monkey : monkeys) {
        while (!monkey.items.empty()) {
            auto item = monkey.items.front();
            monkey.items.pop_front();

            std::vector<int> modulos{};
            for (const auto& m2 : monkeys) {
                modulos.emplace_back(item % m2.divCheck);
            }
            monkey.convertedItems.push_back(modulos);
        }
    }

    auto work2{monkeys};
    for (size_t i{0}; i < 10000; ++i) {
        for (size_t iM{0}; iM < work2.size(); ++iM) {
            auto& monkey = work2[iM];
            while (!monkey.convertedItems.empty()) {
                monkey.inspectionCnt += 1;

                auto item = monkey.convertedItems.front();
                monkey.convertedItems.pop_front();

                for (size_t iV{0}; iV < item.size(); ++iV) {
                    auto& val{item.at(iV)};
                    val = monkey.operation(val);
                    val %= work2.at(iV).divCheck;
                };

                const auto target = monkey.targets[(item[iM] == 0) ? 0 : 1];
                work2[target].convertedItems.emplace_back(std::move(item));
            }
        }
    }

    std::sort(std::begin(work2), std::end(work2), [](const auto& m1, const auto& m2) {
        return m1.inspectionCnt > m2.inspectionCnt;
    });

    fmt::print("{}\n", work1[0].inspectionCnt * work1[1].inspectionCnt);
    fmt::print("{}\n", work2[0].inspectionCnt * work2[1].inspectionCnt);
}
