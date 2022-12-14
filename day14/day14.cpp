#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <algorithm>
#include <limits>

struct Pos {
  int x{};
  int y{};

  auto operator<=>(const Pos& pos) const = default;

  Pos& operator+=(const Pos& other) {
    x += other.x;
    y += other.y;
    return *this;
  }
};

Pos operator+(Pos left, const Pos& right) {
  left += right;
  return left;
}

using Grid = std::map<Pos, char>;

unsigned sand(Grid grid, bool part1) {
  static constexpr std::array<Pos, 3> OFFSETS{Pos{0, 1}, Pos{-1, 1}, Pos{1, 1}};
  Pos start = {500, 0};

  int lowest = std::ranges::max_element(grid, [](const auto& e1, const auto& e2) { return e1.first.y < e2.first.y; })->first.y;

  while (true) {
    Pos pos = start;
    while (true) {
      bool moved = false;
      for (const auto& offset : OFFSETS) {
        if (auto p = pos + offset; !grid.count(p)) {
          pos = p;
          moved = true;
          break;
        }
      }

      if ((pos.y > lowest && part1) || (pos == start && !part1))
        return std::ranges::count_if(grid, [](const auto& e) { return e.second == 'o'; }) + !part1;

      if (!moved || (pos.y == lowest + 1 && !part1)) {
        grid[(pos)] = 'o';
        break;
      }
    }
  }

  return 0;
}

std::istream& operator>>(std::istream& is, Grid& grid) {
  const auto createPath = [&grid](const std::vector<Pos>& path) {
    for (size_t i = 1; i < path.size(); i++) {
      const Pos& start = path[i - 1];
      const Pos& end = path[i];
      Pos p = start;
      int dx = end.x > start.x ? 1 : end.x == start.x ? 0 : -1;
      int dy = end.y > start.y ? 1 : end.y == start.y ? 0 : -1;

      grid[start] = '#';
      while (p != end) {
        p.x += dx;
        p.y += dy;
        grid[p] = '#';
      }
    }
  };

  while (!is.eof() && is.peek() != EOF) {
    std::vector<Pos> path;
    std::string unused;
    char dummy;
    int x, y;

    while (true) {
      is >> x >> dummy >> y;
      path.emplace_back(x, y);
      if (is.peek() == '\n') {
        break;
      } else {
        is >> unused;
      }
    }
    is.ignore();

    createPath(path);
    path.clear();
  }
  return is;
}

int main() {
  Grid grid;

  std::cin >> grid;

  std::cout << "Part 1 result = " << sand(grid, true) << '\n';
  std::cout << "Part 2 result = " << sand(grid, false) << '\n';

  return 0;
}
