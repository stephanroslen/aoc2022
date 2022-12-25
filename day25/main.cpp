#include <fmt/core.h>

#include <iostream>
#include <string>

using IType = long int;

int main()
{
    const auto fromSnafu{[](const std::string& s) {
        IType result{0};
        for (const auto& c : s) {
            result *= 5;
            if (c >= '0' && c <= '2') {
                result += (c - '0');
            } else if (c == '-') {
                result -= 1;
            } else if (c == '=') {
                result -= 2;
            }
        }
        return result;
    }};

    const auto toSnafu{[](IType val) {
        std::string result{};

        while (val != 0) {
            const auto mod{(val + 5) % 5};
            if (mod >= 0 && mod <= 2) {
                result = static_cast<char>(mod + '0') + result;
                val /= 5;
            } else if (mod == 3) {
                result = '=' + result;
                val = (val + 2) / 5;
            } else if (mod == 4) {
                result = '-' + result;
                val = (val + 1) / 5;
            }
        }

        return result;
    }};

    const auto solution([&]() {
        std::string line{};
        IType sum{0};
        while (std::getline(std::cin, line)) {
            sum += fromSnafu(line);
        }
        return toSnafu(sum);
    });

    const auto part1{solution()};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", 0);
}
