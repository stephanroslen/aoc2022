#include <fmt/core.h>

#include <bits/stdc++.h>

enum struct Part { Part1, Part2 };

using ValveName = std::string;

struct Valve {
    ValveName name{};
    int pressure{};
    std::unordered_map<ValveName, int> routes{};
    size_t idx;

    friend std::istream& operator>>(std::istream& is, Valve& valve)
    {
        if (is.good()) {
            valve.routes.clear();

            is.ignore(6);
            valve.name.resize(2);
            is >> valve.name[0];
            is >> valve.name[1];
            is.ignore(15);
            is >> valve.pressure;
            is.ignore(24);
            if (' ' == is.peek()) {
                is.ignore(1);
            }

            for (;;) {
                std::string target{"  "};
                is >> target[0];
                is >> target[1];
                valve.routes.emplace(std::move(target), 1);
                if (!is.good() || is.peek() != ',') {
                    break;
                }
                is.ignore(2);
            }
        }
        return is;
    }
};

struct ValveMap {
    using MapType = std::unordered_map<ValveName, Valve>;
    using DistMap = std::unordered_map<ValveName, std::unordered_map<ValveName, int>>;
    MapType map{};
    DistMap distMap{};

    friend std::istream& operator>>(std::istream& is, ValveMap& vm)
    {
        std::transform(
            std::istream_iterator<Valve>{std::cin},
            std::istream_iterator<Valve>{},
            std::inserter(vm.map, std::begin(vm.map)),
            [i = size_t{0}](Valve valve) mutable {
                valve.idx = i++;
                return std::make_pair(valve.name, valve);
            });
        vm.compress();
        vm.setupDistMap();
        return is;
    }

    void setupDistMap()
    {
        for (const auto& [i, tmp1] : map) {
            for (const auto& [j, tmp2] : map) {
                distMap[i][j] = 1000;
            }
        }
        for (const auto& [n, v] : map) {
            for (const auto& [t, d] : v.routes) {
                distMap.at(n).at(t) = d;
            }
            distMap.at(n).at(n) = 0;
        }
        for (const auto& [k, tmp0] : map) {
            for (const auto& [i, tmp1] : map) {
                for (const auto& [j, tmp2] : map) {
                    auto& dij{distMap.at(i).at(j)};
                    const auto dik{distMap.at(i).at(k)};
                    const auto dkj{distMap.at(k).at(j)};
                    const auto dsum{dik + dkj};
                    if (dij > dsum) {
                        dij = dsum;
                    }
                }
            }
        }
    }

    void fixDist(const std::string& from, const std::string& to, int newDist, const std::string& clear)
    {
        auto& vFrom{map.at(from)};
        vFrom.routes.erase(clear);
        const auto it{vFrom.routes.find(to)};
        if (std::cend(vFrom.routes) != it) {
            auto& dist{it->second};
            dist = std::min(dist, newDist);
        } else {
            vFrom.routes.emplace(to, newDist);
        }
    }

    void compressRemove(MapType::iterator& it)
    {
        const auto& [name, valve]{*it};
        for (const auto& [t1, dist1] : valve.routes) {
            for (const auto& [t2, dist2] : valve.routes) {
                if (t1 != t2) {
                    const auto dist{dist1 + dist2};
                    fixDist(t1, t2, dist, name);
                    fixDist(t2, t1, dist, name);
                }
            }
        }
    }

    void compress()
    {
        for (auto it{std::begin(map)}; std::end(map) != it;) {
            if (it->first != "AA" && it->second.pressure == 0) {
                compressRemove(it);
                it = map.erase(it);
            } else {
                ++it;
            }
        }
    }
};

struct State {
    std::string initLoc;
    int initMovesLeft;
    size_t restarts;

    std::bitset<64> openValves{};

    std::string loc;
    int movesLeft;

    State(std::string vLoc, int vMovesLeft, size_t vRestarts) : initLoc{vLoc}, initMovesLeft{vMovesLeft}, restarts{vRestarts}, loc{vLoc}, movesLeft{vMovesLeft} {};
    State(const State&) = default;
    State(State&&) = default;
    State &operator=(const State&) = default;
    State &operator=(State&&) = default;

    bool operator==(const State& rhs) const
    {
        return std::tie(restarts, openValves, loc, movesLeft)
               == std::tie(rhs.restarts, rhs.openValves, rhs.loc, rhs.movesLeft);
    }

    struct Hasher {
        size_t operator()(const State& state) const
        {
            size_t result{state.openValves.to_ulong()};
            result ^= std::hash<std::string>{}(state.loc);
            result ^= std::hash<int>{}(state.movesLeft);
            result ^= state.restarts;
            return result;
        }
    };

    bool finished(const ValveMap& vm) const
    {
        for (const auto& [name, valve] : vm.map) {
            if (valve.pressure > 0 && !openValves[valve.idx]) {
                return false;
            }
        }
        return true;
    }

    bool openable(const Valve& v) const
    {
        return (v.pressure > 0 && !openValves[v.idx]);
    }

    bool noMovesLeft() const
    {
        return movesLeft <= 1;
    }
};

struct Cache {
    std::unordered_map<State, int, State::Hasher> data{};
};

int solve1(Cache& cache, const ValveMap& vm, const State& state)
{
    if (const auto it{cache.data.find(state)}; std::cend(cache.data) != it) {
        return it->second;
    }

    int result{0};

    const auto& vLoc{vm.map.at(state.loc)};

    if ((state.noMovesLeft() && state.restarts == 0)|| state.finished(vm)) {
        goto out;
    }

    if (state.noMovesLeft() && state.restarts == 1) {
        auto newState{state};
        newState.movesLeft = 26;
        newState.loc = "AA";
        newState.restarts = 0;
        return solve1(cache, vm, newState);
    }

    if (state.openable(vLoc)) {
        auto newState{state};
        newState.movesLeft -= 1;
        newState.openValves.set(vLoc.idx);
        result = std::max(result, vLoc.pressure * newState.movesLeft + solve1(cache, vm, newState));
    }
    for (const auto& [target, dist] : vm.distMap.at(vLoc.name)) {
        if (dist == 0) {
            continue;
        }
        auto newState{state};
        newState.movesLeft -= dist;
        newState.loc = target;
        result = std::max(result, solve1(cache, vm, newState));
    }

out:
    cache.data.emplace(state, result);
    return result;
}

int main()
{
    ValveMap vm{};
    std::cin >> vm;

#if 0
    for (const auto &[a, b] : vm.distMap) {
        for (const auto &[c, d] : b) {
            fmt::print("{} -> {} : {}\n", a, c, d);
        }
    }
#endif

    {
        Cache cache1{};
        State state1{"AA", 30, 0};

        const auto part1{solve1(cache1, vm, state1)};
        fmt::print("{}\n", part1);
    }

    {
        Cache cache1{};
        State state1{"AA", 26, 1};

        const auto part1{solve1(cache1, vm, state1)};
        fmt::print("{}\n", part1);
    }
}
