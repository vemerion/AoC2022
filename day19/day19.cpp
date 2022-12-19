#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <array>
#include <cassert>
#include <numeric>

static constexpr size_t RESOURCE_COUNT = 4;

using Robots = std::array<unsigned, RESOURCE_COUNT>;
using Resources = std::array<unsigned, RESOURCE_COUNT>;
using Blueprint = std::array<Resources, RESOURCE_COUNT>;
using Blueprints = std::vector<Blueprint>;

size_t toIndex(const std::string& resource) {
  if (resource == "ore")
    return 0;
  else if (resource == "clay")
    return 1;
  else if (resource == "obsidian")
    return 2;
  else if (resource == "geode")
    return 3;

  assert(false);
}

struct State {
  Robots robots{};
  Resources resources{};
};

std::istream& operator>>(std::istream& is, Blueprints& blueprints) {
  const auto skip = [&is](unsigned count) {
    std::string s;
    for (unsigned i = 0; i < count; i++) is >> s;
  };

  const auto add = [](Blueprint& blueprint, const std::string& robot, std::string& resource, unsigned cost) {
    if (resource.back() == '.')
      resource.pop_back();
    blueprint[toIndex(robot)][toIndex(resource)] = cost;
  };

  std::string robot, resource1, resource2, unused;
  unsigned cost1, cost2;

  while (!is.eof() && is.peek() != EOF) {
    Blueprint blueprint{};
    skip(3);

    // Ore
    is >> robot >> unused >> unused >> cost1 >> resource1;
    add(blueprint, robot, resource1, cost1);


    // Clay
    is >> unused >> robot >> unused >> unused >> cost1 >> resource1;
    add(blueprint, robot, resource1, cost1);

    // Obsidian
    is >> unused >> robot >> unused >> unused >> cost1 >> resource1 >> unused >> cost2 >> resource2;
    add(blueprint, robot, resource1, cost1);
    add(blueprint, robot, resource2, cost2);

    // Geode
    is >> unused >> robot >> unused >> unused >> cost1 >> resource1 >> unused >> cost2 >> resource2;
    add(blueprint, robot, resource1, cost1);
    add(blueprint, robot, resource2, cost2);

    blueprints.push_back(blueprint);
    is.ignore();
  }

  return is;
}

void collectGeodes(const Blueprint& blueprint, State state, unsigned time, unsigned& best, const Robots& max) {

  if (auto a = state.robots[toIndex("geode")] + time; state.resources[toIndex("geode")] + (a * (a + 1)) / 2 <= best) {
    return;
  }

  const auto harvest = [](const Robots& robots, Resources& resources) {
    for (size_t robot = 0; robot < robots.size(); robot++)
      resources[robot] += robots[robot];
  };

  // Base case
  if (time == 0) {
    if (auto geodes = state.resources[toIndex("geode")]; geodes > best) {
      best = geodes;
    }
    return;
  }

  // Build robot
  for (size_t robot = 0; robot < blueprint.size(); robot++) {
    const auto& cost = blueprint[robot];
    if (robot != toIndex("geode") && max[robot] == state.robots[robot])
      continue;

    bool canBuild = true;
    for (size_t resource = 0; resource < cost.size(); resource++) {
      const auto& count = cost[resource];
      if (state.resources[resource] < count) {
        canBuild = false;
        break;
      }
    }

    if (canBuild) {
      State s = state;
      for (size_t resource = 0; resource < cost.size(); resource++)
        s.resources[resource] -= cost[resource];
      s.robots[robot]++;

      harvest(state.robots, s.resources);

      collectGeodes(blueprint, s, time - 1, best, max);
    }
  }

  harvest(state.robots, state.resources);

  collectGeodes(blueprint, state, time - 1, best, max);
}


auto collectGeodes(const Blueprints& blueprints, unsigned count, unsigned time) {
  std::vector<unsigned> result;

  for (unsigned i = 0; i < count; i++) {
    const auto& blueprint = blueprints[i];
    unsigned geodes = 0;

    State start;
    start.robots[toIndex("ore")] = 1;

    Robots max{};
    for (size_t robot = 0; robot < blueprint.size(); robot++) {
      const auto& cost = blueprint[robot];
      for (size_t resource = 0; resource < cost.size(); resource++)
        if (const auto count = cost[resource]; count > max[resource])
          max[resource] = count;
    }

    collectGeodes(blueprint, start, time, geodes, max);

    result.push_back(geodes);
  }

  return result;
}

unsigned part1(const Blueprints& blueprints) {
  unsigned result = 0;
  const auto geodes = collectGeodes(blueprints, blueprints.size(), 24);
  for (size_t i = 0; i < geodes.size(); i++)
    result += (i + 1) * geodes[i];

  return result;
}

unsigned part2(const Blueprints& blueprints) {
  const auto geodes = collectGeodes(blueprints, 3, 32);

  return std::accumulate(geodes.begin(), geodes.end(), 1, [](const auto& a, const auto& b) { return a * b; });
}

int main() {
  Blueprints blueprints;

  std::cin >> blueprints;

  std::cout << "Part 1 result = " << part1(blueprints) << '\n';
  std::cout << "Part 2 result = " << part2(blueprints) << '\n';

  return 0;
}
