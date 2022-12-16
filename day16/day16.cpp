#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <string>
#include <numeric>

struct Valve {
  std::string id;
  unsigned pressure;
  std::vector<std::string> adjacent;
};

using Cave = std::map<std::string, Valve>;

std::istream& operator>>(std::istream& is, Cave& cave) {
  std::string unused;

  while (!is.eof() && is.peek() != EOF) {
    Valve valve;
    is.ignore(5);
    is >> valve.id;
    is.ignore(15);
    is >> valve.pressure;
    is >> unused >> unused >> unused >> unused >> unused;

    std::string id;
    while (is.peek() != '\n' && is.peek() != EOF) {
      is >> id;
      valve.adjacent.push_back(id.substr(0, 2));
    }
    is.ignore();
    cave[valve.id] = valve;
  }
  return is;
}

struct Agent {
  std::string current;
  unsigned walking{};
};

void releasePressure(const Cave& cave, std::set<std::string> opened, Agent you, Agent elephant, unsigned time, unsigned pressure, unsigned& best) {
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

  struct Move {
    const Valve* valve{};
    const unsigned time{};
  };

  const auto findPaths = [&cave, &opened](const Agent& agent) -> std::list<Move> {
    if (agent.walking > 0)
      return {Move{&cave.at(agent.current), agent.walking}};

    std::list<Move> list{Move{&cave.at(agent.current), 1}};
    std::set<const Valve*> visited{&cave.at(agent.current)};
    std::list<Move> result;
    while (!list.empty()) {
      const auto move = list.front();
      list.pop_front();

      if (move.valve->pressure != 0 && !opened.count(move.valve->id))
        result.push_back(move);
      for (const auto& id : move.valve->adjacent) {
        if (!visited.count(&cave.at(id))) {
          visited.insert(&cave.at(id));
          list.emplace_back(&cave.at(id), move.time + 1);
        }
      }
    }
    return result;
  };

  auto yourPaths = findPaths(you);
  auto elephantPaths = findPaths(elephant);

  for (const auto& [v1, t1] : yourPaths) {
    if (t1 > time)
      continue;

    for (const auto& [v2, t2] : elephantPaths) {
      if (t2 > time || v1->id == v2->id)
        continue;

      releasePressure(cave, opened, Agent{v1->id, t1}, Agent{v2->id, t2}, time, pressure, best);
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

  releasePressure(cave, {"AA"}, Agent{"AA", 0}, Agent{"AA", 30}, 30, 0, best);

  return best;
}

unsigned part2(const Cave& cave) {
  unsigned best = 0;

  releasePressure(cave, {"AA"}, Agent{"AA", 0}, Agent{"AA", 0}, 26, 0, best);

  return best;
}

int main() {
  Cave cave;

  std::cin >> cave;

  std::cout << "Part 1 result = " << part1(cave) << '\n';
  std::cout << "Part 2 result = " << part2(cave) << '\n';

  return 0;
}
