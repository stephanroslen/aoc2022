#include <fmt/core.h>

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

struct Directory;

struct INode {
    explicit INode(std::string nameVal) noexcept
        : name{std::move(nameVal)} {};

    std::string name{};

    virtual void eachDir(const std::function<void(const Directory&)>& visitor) const noexcept = 0;

    virtual size_t fullSize() const noexcept = 0;

    virtual ~INode() noexcept = default;
};

struct File : public INode {
    File(std::string nameVal, size_t sizeVal) noexcept
        : INode{std::move(nameVal)}
        , size{sizeVal} {};
    size_t size{0};

    size_t fullSize() const noexcept override
    {
        return size;
    }

    void eachDir(const std::function<void(const Directory&)>&) const noexcept override
    {}
};

struct Directory : public INode {
    Directory(std::string nameVal) noexcept
        : INode{std::move(nameVal)} {};

    std::vector<std::unique_ptr<INode>> nodes{};

    size_t fullSize() const noexcept override
    {
        return std::transform_reduce(std::cbegin(nodes), std::cend(nodes), size_t{0}, std::plus{}, [](const auto& node) {
            return node->fullSize();
        });
    }

    void eachDir(const std::function<void(const Directory&)>& visitor) const noexcept override
    {
        visitor(*this);
        for (const auto& node : nodes) {
            node->eachDir(visitor);
        }
    }
};

std::unique_ptr<INode> readDirectory(std::istream& is, std::string name)
{
    std::string input{};

    if ("/" == name) {
        std::getline(std::cin, input);
        assert("$ cd /" == input);
    }
    auto result{std::make_unique<Directory>(std::move(name))};

    std::getline(is, input);
    assert("$ ls" == input);

    std::queue<std::string> subdirs{};

    while (is && '$' != is.peek()) {
        if (!std::getline(is, input)) {
            break;
        }
        if (input.starts_with("dir ")) {
            subdirs.push(input.substr(4));
        } else {
            const auto spaceloc{input.find(' ')};
            const auto size{std::stoul(input.substr(0, spaceloc))};
            auto fileName{input.substr(spaceloc + 1)};
            result->nodes.emplace_back(std::make_unique<File>(std::move(fileName), size));
        }
    }

    while (!subdirs.empty()) {
        auto& sd{subdirs.front()};
        std::getline(is, input);
        assert("$ cd " + sd == input);

        result->nodes.emplace_back(readDirectory(is, sd));

        subdirs.pop();
    }

    if (std::getline(is, input)) {
        assert("$ cd .." == input);
    }

    return result;
}

int main()
{
    const auto rootDir{readDirectory(std::cin, "/")};

    const auto part1{[&]() {
        constexpr size_t part1Limit{100000};

        size_t result{0};
        rootDir->eachDir([&](const Directory& dir) {
            const auto fs{dir.fullSize()};
            if (fs <= part1Limit) {
                result += fs;
            }
        });

        return result;
    }()};

    const auto part2{[&]() {
        constexpr size_t totalSpace{70000000};
        constexpr size_t neededSpace{30000000};

        const size_t unusedSpace{totalSpace - rootDir->fullSize()};
        const size_t missingSpace{neededSpace - unusedSpace};

        size_t result{std::numeric_limits<size_t>::max()};
        rootDir->eachDir([&](const Directory& dir) {
            const auto fs{dir.fullSize()};
            if (fs >= missingSpace && fs < result) {
                result = fs;
            }
        });

        return result;
    }()};

    fmt::print("{}\n", part1);
    fmt::print("{}\n", part2);
}
