#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <string>
#include <numeric>

struct Valve {
  unsigned pressure;
  std::map<unsigned, unsigned> connections;
};

using Cave = std::map<unsigned, Valve>;

std::istream& operator>>(std::istream& is, Cave& cave) {
  std::string unused;

  struct Node {
    std::string name;
    unsigned id;
    unsigned pressure;
    std::vector<std::string> adjacent;
  };
  std::map<std::string, Node> nodes;

  while (!is.eof() && is.peek() != EOF) {
    Node node;
    is.ignore(5);
    is >> node.name;
    is.ignore(15);
    is >> node.pressure;
    is >> unused >> unused >> unused >> unused >> unused;

    std::string name;
    while (is.peek() != '\n' && is.peek() != EOF) {
      is >> name;
      node.adjacent.push_back(name.substr(0, 2));
    }
    is.ignore();
    nodes[node.name] = node;
  }

  // Assign ids
  unsigned id = 0;
  for (auto& [name, node] : nodes)
    node.id = id++;

  // Create caves
  const auto BFS = [&nodes](const Node& node) -> std::map<unsigned, unsigned> {
    struct Entry {
      std::string name;
      unsigned cost;
    };
    std::list<Entry> list{Entry{node.name, 1}};
    std::set<std::string> visited{node.name};
    std::map<unsigned, unsigned> result;
    while (!list.empty()) {
      const auto [connection, cost] = list.front();
      list.pop_front();

      for (const auto& name : nodes[connection].adjacent) {
        if (!visited.count(name)) {
          visited.insert(name);
          list.push_back({name, cost + 1});
          if (nodes[name].pressure != 0)
            result[nodes[name].id] = cost + 1;
        }
      }
    }

    return result;
  };

  for (const auto& [name, node] : nodes) {
    cave[node.id].connections = BFS(node);
    cave[node.id].pressure = node.pressure;
  }

  return is;
}

struct Agent {
  unsigned current{};
  unsigned walking{};
};

void releasePressure(const Cave& cave, std::set<unsigned> opened, Agent you, Agent elephant, unsigned time, unsigned pressure, unsigned& best) {
  static const unsigned TOTAL_PRESSURE = std::accumulate(cave.begin(), cave.end(), 0, [](const auto& a, const auto&b) { return a + b.second.pressure; });
  if (pressure + TOTAL_PRESSURE * time <= best) {
    return;
  }

  const auto steps = std::min(you.walking, elephant.walking);
  pressure += steps * std::accumulate(opened.begin(), opened.end(), 0, [&cave](const auto& a, const auto& b) { return a + cave.at(b).pressure; });
  you.walking -= steps;
  elephant.walking -= steps;
  time -= steps;

  if (you.walking == 0)
    opened.insert(you.current);
  if (elephant.walking == 0)
    opened.insert(elephant.current);

  const auto findPaths = [&cave, &opened](const Agent& agent) -> std::map<unsigned, unsigned> {
    if (agent.walking > 0)
      return {{agent.current, agent.walking}};

    std::map<unsigned, unsigned> result;
    for (const auto& [id, cost] : cave.at(agent.current).connections)
      if (!opened.count(id))
        result[id] = cost;
    return result;
  };

  for (const auto& [v1, t1] : findPaths(you)) {
    if (t1 > time)
      continue;

    for (const auto& [v2, t2] : findPaths(elephant)) {
      if (t2 > time || v1 == v2)
        continue;

      releasePressure(cave, opened, Agent{v1, t1}, Agent{v2, t2}, time, pressure, best);
    }
  }

  if (you.walking > 0 || elephant.walking > 0) {
    const auto steps = std::max(you.walking, elephant.walking);
    pressure += steps * std::accumulate(opened.begin(), opened.end(), 0, [&cave](const auto& a, const auto& b) { return a + cave.at(b).pressure; });
    time -= steps;
    opened.insert(you.current);
    opened.insert(elephant.current);
  }

  pressure += time * std::accumulate(opened.begin(), opened.end(), 0, [&cave](const auto& a, const auto& b) { return a + cave.at(b).pressure; });
  if (pressure > best) {
    best = pressure;
  }
}

unsigned part1(const Cave& cave) {
  unsigned best = 0;

  releasePressure(cave, {0}, Agent{0, 0}, Agent{0, 30}, 30, 0, best);

  return best;
}

unsigned part2(const Cave& cave) {
  unsigned best = 0;

  releasePressure(cave, {0}, Agent{0, 0}, Agent{0, 0}, 26, 0, best);

  return best;
}

int main() {
  Cave cave;

  std::cin >> cave;

  std::cout << "Part 1 result = " << part1(cave) << '\n';
  std::cout << "Part 2 result = " << part2(cave) << '\n';

  return 0;
}
