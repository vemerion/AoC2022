#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <cassert>

struct Pos {
  int64_t row{};
  int64_t col{};

  auto operator<=>(const Pos& pos) const = default;

  Pos& operator+=(const Pos& other) {
    row += other.row;
    col += other.col;
    return *this;
  }

  Pos& operator-=(const Pos& other) {
    row -= other.row;
    col -= other.col;
    return *this;
  }
};

Pos operator+(Pos left, const Pos& right) {
  left += right;
  return left;
}

Pos operator-(Pos left, const Pos& right) {
  left -= right;
  return left;
}

struct Tile {
  std::vector<Pos> blizzards;
  bool wall{};

  auto operator<=>(const Tile&) const = default;

  bool empty() const {
    return blizzards.empty() && !wall;
  }
};

using Tiles = std::vector<std::vector<Tile>>;

class Grid {
public:
  std::pair<unsigned, unsigned> path() const {
    static constexpr std::array<Pos, 5> OFFSETS{Pos{0, 0}, Pos{-1, 0}, Pos{1, 0}, Pos{0, -1}, Pos{0, 1}};

    // A node occupies a (row, col, time) position
    struct Pos3 {
      Pos pos{};
      unsigned time{};

      auto operator<=>(const Pos3&) const = default;
    };
    struct Node {
      std::vector<Pos3> edges;
      bool visited{};
      unsigned value{};
    };
    std::map<Pos3, Node> graph;

    // Create grids to build graph
    std::vector<Tiles> grids{grid};
    Tiles current = grid;
    Tiles next = current;
    for (unsigned time = 0;; time++) {
      // Clear next
      for (auto& row : next)
        for (auto& tile : row)
          tile.blizzards.clear();

      // Update next
      for (unsigned row = 0; row < current.size(); row++) {
        for (unsigned col = 0; col < current[row].size(); col++) {
          for (const auto& blizzard : current[row][col].blizzards) {
            auto moved = Pos(row, col) + blizzard;
            if (next[moved.row][moved.col].wall) {
              for (moved = Pos(row, col); !next[moved.row - blizzard.row][moved.col - blizzard.col].wall; moved -= blizzard)
                ;
            }
            next[moved.row][moved.col].blizzards.push_back(blizzard);
          }
        }
      }

      bool repeated = next == grid;

      // Build graph
      for (unsigned row = 0; row < current.size(); row++) {
        for (unsigned col = 0; col < current[row].size(); col++) {
          if (!current[row][col].empty())
            continue;

          for (const auto& offset : OFFSETS) {
            Pos p = Pos(row, col) + offset;
            if (p.row >= 0 && static_cast<size_t>(p.row) < current.size() && p.col >= 0 && static_cast<size_t>(p.col) < current[p.row].size() && next[p.row][p.col].empty()) {
              graph[{row, col, time}].edges.emplace_back(p, repeated ? 0 : time + 1);
            }
          }
        }
      }

      current = next;

      if (repeated)
        break;
    }

    const auto BFS = [&graph](const Pos3& start, const Pos& end) -> unsigned {
      std::list<Node*> list{&graph[start]};
      
      // Reset
      graph[start].value = start.time;
      for (auto& [pos, node] : graph)
        node.visited = false;

      while (!list.empty()) {
        auto* current = list.front();
        list.pop_front();

        for (const auto& edge : current->edges) {
          if (edge.pos == end) {
            return current->value + 1;
          }

          auto& neighbor = graph[edge];
          if (neighbor.visited)
            continue;
          neighbor.visited = true;
          neighbor.value = current->value + 1;
          list.push_back(&neighbor);
        }
      }

      assert(false);
    };

    auto time1 = BFS({start.row, start.col, 0}, end);
    auto time2 = BFS({end.row, end.col, time1}, start);
    auto time3 = BFS({start.row, start.col, time2}, end);
    return {time1, time3};
  }

friend std::istream& operator>>(std::istream& is, Grid& grid);

private:
  Tiles grid;
  Pos start;
  Pos end;
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  static const std::map<char, Pos> BLIZZARDS{{'>', Pos{0, 1}}, {'v', Pos{1, 0}}, {'<', Pos{0, -1}}, {'^', Pos{-1, 0}}};

  std::string line;

  while (is >> line) {
    grid.grid.emplace_back();
    for (unsigned col = 0; col < line.size(); col++) {
      if (line[col] == '#') {
        grid.grid.back().emplace_back(std::vector<Pos>{}, true);
      } else if (line[col] == '.') {
        grid.grid.back().emplace_back(std::vector<Pos>{}, false);
      } else {
        grid.grid.back().emplace_back(std::vector<Pos>{BLIZZARDS.at(line[col])}, false);
      }
    }
  }

  for (unsigned col = 0; col < grid.grid.front().size(); col++) {
    if (!grid.grid.front()[col].wall) {
      grid.start = Pos(0, col);
    }
    if (!grid.grid.back()[col].wall) {
      grid.end = Pos(grid.grid.size() - 1, col);
    }
  }

  return is;
}

int main() {
  Grid grid;

  std::cin >> grid;

  const auto [part1, part2] = grid.path();
  std::cout << "Part 1 result = " << part1 << '\n';
  std::cout << "Part 2 result = " << part2 << '\n';

  return 0;
}
