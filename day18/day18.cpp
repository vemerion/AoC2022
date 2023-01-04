#include <iostream>
#include <string>
#include <unordered_set>
#include <list>
#include <array>
#include <cmath>
#include <algorithm>
#include <numeric>

struct Pos {
  int x{};
  int y{};
  int z{};

  auto operator<=>(const Pos&) const = default;

  Pos& operator+=(const Pos& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  unsigned distance(const Pos& other) const {
    return std::abs(x - other.x) + std::abs(y - other.y) + std::abs(z - other.z);
  }
};

Pos operator+(Pos left, const Pos& right) {
  left += right;
  return left;
}

template<>
struct std::hash<Pos> {
  size_t operator()(const Pos& pos) const {
    return 31 * pos.x + 37 * pos.y + 41 * pos.z;
  }
};

using Droplet = std::unordered_set<Pos>;

std::istream& operator>>(std::istream& is, Droplet& droplet) {
  Pos pos;
  char unused;

  while (is >> pos.x >> unused >> pos.y >> unused >> pos.z)
    droplet.insert(pos);

  return is;
}

unsigned surfaceArea(const Droplet& droplet) {
  unsigned count = 0;
  for (const auto& p1 : droplet) {
    unsigned sides = 6;
    for (const auto& p2 : droplet) {
      if (p1.distance(p2) != 1)
        continue;
      sides--;
    }
    count += sides;
  }

  return count;
}

unsigned exteriorSurface(const Droplet& droplet) {
  static constexpr std::array<Pos, 6> OFFSETS{Pos{-1, 0, 0}, Pos{1, 0, 0}, Pos{0, -1, 0}, Pos{0, 1, 0}, Pos{0, 0, -1}, Pos{0, 0, 1}};

  const auto findOutside = [&droplet](const Pos& start, unsigned max) -> std::unordered_set<Pos> {
    if (droplet.count(start))
      return {};

    std::list<Pos> list{start};
    std::unordered_set<Pos> visited{start};

    while (!list.empty()) {
      const auto pos = list.front();
      list.pop_front();

      if (visited.size() > max) {
        return visited;
      }

      for (const auto& offset : OFFSETS) {
        if (const Pos p = pos + offset; !droplet.count(p) && !visited.count(p)) {
          visited.insert(p);
          list.push_back(p);
        }
      }
    }

    return {};
  };

  unsigned maxSide = std::sqrt(droplet.size() / 6);
  unsigned maxVolume = maxSide * maxSide * maxSide;

  unsigned count = 0;
  std::unordered_set<Pos> outside;

  for (const auto& pos : droplet) {
    for (const auto& offset : OFFSETS) {
      const auto p = pos + offset;
      if (outside.count(p)) {
        count++;
        continue;
      }
      auto found = findOutside(p, maxVolume);
      outside.insert(found.begin(), found.end());
      count += !found.empty();
    }
  }

  return count;
}

int main() {
  Droplet droplet;

  std::cin >> droplet;
  std::cout << "Part 1 result = " << surfaceArea(droplet) << '\n';
  std::cout << "Part 2 result = " << exteriorSurface(droplet) << '\n';

  return 0;
}
