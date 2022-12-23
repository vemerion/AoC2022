#include <iostream>
#include <map>
#include <set>
#include <array>
#include <string>
#include <algorithm>

struct Pos {
  int64_t x{};
  int64_t y{};

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

using Grid = std::set<Pos>;

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line;
  unsigned y = 0;

  while (is >> line) {
    for (unsigned x = 0; x < line.size(); x++) {
      if (line[x] == '#') {
        grid.insert(Pos(x, y));
      }
    }
    y++;
  }
  return is;
}

unsigned spread(Grid grid, bool part1) {
  const auto isAlone = [&grid](const auto& pos) -> bool {
    for (int x = -1; x < 2; x++) {
      for (int y = -1; y < 2; y++) {
        if (Pos p = {pos.x + x, pos.y + y}; pos != p && grid.count(p)) {
          return false;
        }
      }
    }
    return true;
  };

  const auto rectangle = [&grid]() -> unsigned {
    Pos topLeft{};
    Pos bottomRight{};

    for (const auto& p : grid) {
      if (p.x < topLeft.x)
        topLeft.x = p.x;
      else if (p.x > bottomRight.x)
        bottomRight.x = p.x;
      if (p.y < topLeft.y)
        topLeft.y = p.y;
      else if (p.y > bottomRight.y)
        bottomRight.y = p.y;
    }

    return (bottomRight.x + 1 - topLeft.x) * (bottomRight.y + 1 - topLeft.y) - grid.size();
  };

  static constexpr std::array<std::array<Pos, 3>, 4> DIRECTIONS{
    std::array<Pos, 3>{Pos{0, -1}, Pos{-1, -1}, Pos{1, -1}},
    std::array<Pos, 3>{Pos{0, 1}, Pos{1, 1}, Pos{-1, 1}},
    std::array<Pos, 3>{Pos{-1, 0}, Pos{-1, -1}, Pos{-1, 1}},
    std::array<Pos, 3>{Pos{1, 0}, Pos{1, -1}, Pos{1, 1}},
  };

  for (unsigned round = 0; part1 ? round < 10 : true; round++) {
    std::map<Pos, unsigned> moveCounts;
    std::map<Pos, Pos> moves;

    // Consider moves
    for (const auto& elf : grid) {

      // Do nothing if alone
      if (isAlone(elf)) {
        continue;
      }

      // Go through directions
      for (unsigned i = 0; i < DIRECTIONS.size(); i++) {
        const auto& OFFSETS = DIRECTIONS[(round + i) % DIRECTIONS.size()];
        if (std::ranges::none_of(OFFSETS, [&grid, &elf](const auto& offset) { return grid.count(elf + offset); })) {
          auto move = elf + OFFSETS[0];
          moves[elf] = move;
          moveCounts[move]++;
          break;
        }
      }
    }

    if (moves.empty() && !part1) {
      return round + 1;
    }

    // Move
    Grid updated;
    for (const auto& elf : grid) {
      if (moves.count(elf)) {
        const auto& move = moves[elf];
        if (moveCounts[move] > 1) {
          updated.insert(elf);
        } else {
          updated.insert(move);
        }
      } else {
        updated.insert(elf);
      }
    }
    grid = std::move(updated);
  }

  return rectangle();
}

int main() {
  Grid grid;

  std::cin >> grid;

  std::cout << "Part 1 result = " << spread(grid, true) << '\n';
  std::cout << "Part 2 result = " << spread(grid, false) << '\n';

  return 0;
}
