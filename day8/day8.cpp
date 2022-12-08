#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <functional>

struct Pos {
  int row{};
  int col{};

  auto operator<=>(const Pos& pos) const = default;

  Pos& operator+=(const Pos& other) {
    row += other.row;
    col += other.col;
    return *this;
  }
};

Pos operator+(Pos left, const Pos& right) {
  left += right;
  return left;
}

class Grid {
public:
  friend std::istream& operator>>(std::istream& is, Grid& grid);

  bool inside(const Pos& pos) const {
    return pos.row >= 0 && static_cast<size_t>(pos.row) < grid.size() && pos.col >= 0 && static_cast<size_t>(pos.col) < grid[pos.row].size();
  }

  char at(const Pos& pos) const {
    return grid[pos.row][pos.col];
  }

  unsigned visibleFromOutside() const {
    unsigned count = 0;

    foreach([&count, this](Pos current) {
      const auto height = at(current);
      for (const auto& offset : OFFSETS) {
        for (auto pos = current + offset;; pos += offset) {
          if (!inside(pos)) {
            count++;
            return;
          } else if (at(pos) >= height) {
            break;
          }
        }
      }
    });

    return count;
  }


  unsigned maxScenicScore() const {
    unsigned max = 0;

    foreach([&max, this](Pos current) {
      if (auto score = scenicScore(current); score > max)
        max = score;
    });

    return max;
  }

private:
  static constexpr std::array<Pos, 4> OFFSETS{Pos{1, 0}, Pos{-1, 0}, Pos{0, 1}, Pos{0, -1}};

  void foreach(const std::function<void(Pos)>& f) const {
    for (size_t row = 0; row < grid.size(); row++) {
      for (size_t col = 0; col < grid[row].size(); col++) {
        f(Pos(row, col));
      }
    }
  }

  unsigned scenicScore(const Pos& current) const {
    unsigned score = 1;

    for (const auto& offset : OFFSETS) {
      unsigned viewing = 0;
      auto height = at(current);
      for (auto pos = current + offset; inside(pos); pos += offset) {
        viewing++;
        if (at(pos) >= height)
          break;
      }
      score *= viewing;
    }

    return score;
  }

  std::vector<std::string> grid;
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  std::string line;

  while (is >> line)
    grid.grid.push_back(line);

  return is;
}


int main() {
  Grid grid;

  std::cin >> grid;

  std::cout << "Part 1 result = " << grid.visibleFromOutside() << '\n';
  std::cout << "Part 2 result = " << grid.maxScenicScore() << '\n';

  return 0;
}
