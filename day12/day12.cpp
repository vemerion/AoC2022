#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <array>
#include <limits>

struct Pos {
  int row{};
  int col{};

  auto operator<=>(const Pos& pos) const = default;
};

struct Tile {
  const Pos pos{};
  const char height{};
  unsigned steps{};
  bool visited{};
};

class Grid {
public:
  unsigned fewestSteps() {
    return fewestSteps(start);
  }

  unsigned hikingTrail() {
    unsigned fewest = std::numeric_limits<unsigned>::max();

    for (auto& row : grid) {
      for (auto& tile : row) {
        if (tile.height == 'a') {
          auto steps = fewestSteps(tile.pos);
          if (steps < fewest)
            fewest = steps;
        }
      }
    }

    return fewest;
  }
  
  friend std::istream& operator>>(std::istream& is, Grid& grid);
private:
  void reset() {
    for (auto& row : grid)
      for (auto& tile : row) {
        tile.steps = 0;
        tile.visited = false;
      }
  }

  unsigned fewestSteps(const Pos& s) {
    unsigned result = std::numeric_limits<unsigned>::max();

    std::list<Tile*> list{&at(s)};
    at(s).visited = true;

    while (!list.empty()) {
      auto* current = list.front();
      list.pop_front();

      for (auto* tile : neighbors(current->pos)) {
        if (tile->visited)
          continue;
        tile->visited = true;
        list.push_back(tile);
        tile->steps = current->steps + 1;
      }

      if (current->pos == end) {
        result = current->steps;
        break;
      }
    }

    reset();
    return result;
  }

  std::vector<Tile*> neighbors(const Pos& pos) {
    static constexpr std::array<Pos, 4> OFFSETS{Pos{0, -1}, Pos{0, 1}, Pos{-1, 0}, Pos{1, 0}};

    std::vector<Tile*> result;

    for (const auto& offset : OFFSETS) {
      auto p = Pos{pos.row + offset.row, pos.col + offset.col};
      if (inside(p) && at(pos).height + 1 >= at(p).height)
        result.push_back(&at(p));
    }

    return result;
  }
  

  bool inside(const Pos& pos) const {
    return pos.row >= 0 && static_cast<size_t>(pos.row) < grid.size() && pos.col >= 0 && static_cast<size_t>(pos.col) < grid[pos.row].size();
  }

  Tile& at(const Pos& pos) {
    return grid[pos.row][pos.col];
  }

  std::vector<std::vector<Tile>> grid;
  Pos start;
  Pos end;
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line;

  while (is >> line) {
    grid.grid.emplace_back();
    for (size_t col = 0; col < line.size(); col++) {
      char c = line[col];
      auto pos = Pos(grid.grid.size() - 1, col);
      if (c == 'S') {
        grid.start = pos;
        c = 'a';
      } else if (c == 'E') {
        grid.end = pos;
        c = 'z';
      }
      grid.grid.back().emplace_back(pos, c);
    }
  }

  return is;
}


int main() {
  Grid grid;

  std::cin >> grid;

  std::cout << "Part 1 result = " << grid.fewestSteps() << '\n';
  std::cout << "Part 2 result = " << grid.hikingTrail() << '\n';

  return 0;
}
