#include <iostream>
#include <set>
#include <array>

struct Pos {
  int x{};
  int y{};

  auto operator<=>(const Pos& pos) const = default;

  Pos& operator+=(const Pos& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  bool adjacent(const Pos& other) {
    return std::abs(x - other.x) <= 1 && std::abs(y - other.y) <= 1;
  }

  void moveTowards(const Pos& other) {
    if (adjacent(other))
      return;

    if (x != other.x) {
      x += x < other.x ? 1 : -1;
    }
    if (y != other.y) {
      y += y < other.y ? 1 : -1;
    }
  }
};

Pos operator+(Pos left, const Pos& right) {
  left += right;
  return left;
}

template <size_t size>
class Rope {
public:
  void move(char c, unsigned amount) {
    for (unsigned i = 0; i < amount; i++) {
      switch (c) {
        case 'U':
          knots.front().y--;
          break;
        case 'R':
          knots.front().x++;
          break;
        case 'D':
          knots.front().y++;
          break;
        case 'L':
          knots.front().x--;
          break;
        default:
          break;
      }

      for (size_t i = 1; i < knots.size(); i++)
        knots[i].moveTowards(knots[i - 1]);
      visited.insert(knots.back());
    }
  }

  size_t visitCount() const {
    return visited.size();
  }

private:
  std::array<Pos, size> knots{};
  std::set<Pos> visited{Pos{}};
};

int main() {
  char direction;
  unsigned amount;
  Rope<2> rope1;
  Rope<10> rope2;

  while (std::cin >> direction >> amount) {
    rope1.move(direction, amount);
    rope2.move(direction, amount);
  }

  std::cout << "Part 1 result = " << rope1.visitCount() << '\n';
  std::cout << "Part 2 result = " << rope2.visitCount() << '\n';
  return 0;
}
