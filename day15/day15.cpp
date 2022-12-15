#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <numeric>

struct Range {
  int64_t min{}; // Inclusive
  int64_t max{}; // Exclusive

  bool empty() const {
    return min >= max;
  }

  int64_t size() const {
    return empty() ? 0 : max - min;
  }

  void expand(Range& other) {
    if (empty() || other.empty() || other.max <= min || other.min >= max)
      return;

    if (other.max >= min)
      min = std::min(min, other.min);
    if (other.min <= max)
      max = std::max(max, other.max);

    other.max = other.min;
  }
};

struct Pos {
  int64_t x{};
  int64_t y{};

  auto operator<=>(const Pos& pos) const = default;

  Pos& operator+=(const Pos& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  int64_t manhattan(const Pos& other) const {
    return std::abs(other.x - x) + std::abs(other.y - y);
  }
};

Pos operator+(Pos left, const Pos& right) {
  left += right;
  return left;
}

struct Sensor {
  Pos pos;
  Pos beacon;
};

using Grid = std::vector<Sensor>;

std::istream& operator>>(std::istream& is, Grid& grid) {
  Sensor sensor;

  while (!is.eof() && is.peek() != EOF) {
    is.ignore(12);
    is >> sensor.pos.x;
    is.ignore(4);
    is >> sensor.pos.y;
    is.ignore(25);
    is >> sensor.beacon.x;
    is.ignore(4);
    is >> sensor.beacon.y;
    is.ignore();
    grid.push_back(sensor);
  }

  return is;
}

auto findRanges(const Grid& grid, int64_t y) {
  std::vector<Range> ranges;

  for (const auto& [pos, beacon] : grid) {
    auto dist = pos.manhattan(beacon);
    if (pos.y - dist > y || pos.y + dist < y)
      continue;

    if (y < pos.y)
      dist = -dist;
    auto width = std::abs((pos.y + dist - y) * 2) + 1;
    Range range = {pos.x - width / 2, pos.x + width / 2 + 1};
    for (auto& r : ranges) {
      r.expand(range);
      if (range.empty())
        break;
    }
    if (!range.empty())
      ranges.push_back(range);
  }

  return ranges;
}

void simplify(std::vector<Range>& ranges) {
  for (size_t i = 0; i < ranges.size(); i++) {
    for (size_t j = i + 1; j < ranges.size(); j++) {
      ranges[i].expand(ranges[j]);
    }
  }
  const auto removed = std::ranges::remove_if(ranges, [](const auto& r) { return r.empty(); });
  ranges.erase(removed.begin(), removed.end());
  std::ranges::sort(ranges, [](const auto& a, const auto& b) { return a.min < b.min; });
}

unsigned part1(const Grid& grid, int64_t y) {
  auto ranges = findRanges(grid, y);
  simplify(ranges);

  std::set<Pos> beacons;
  for (const auto& [p, b] : grid)
    beacons.insert(b);
  return std::accumulate(ranges.begin(), ranges.end(), 0, [](const auto& a, const auto& b) { return a + b.size(); }) - std::ranges::count_if(beacons, [y](const auto& b) { return b.y == y; });
}

int64_t part2(const Grid& grid, int64_t max) {
  for (int64_t y = 0; y <= max; y++) {
    auto ranges = findRanges(grid, y);
    simplify(ranges);
    for (size_t i = 1; i < ranges.size(); i++)
      if (int64_t x = ranges[i].min - 1; x >= 0 && x <= max && ranges[i - 1].max == x)
        return x * 4000000 + y;
  }

  return 0;
}

int main() {
  Grid grid;

  std::cin >> grid;

  std::cout << "Part 1 result = " << part1(grid, 2000000) << '\n';
  std::cout << "Part 2 result = " << part2(grid, 4000000) << '\n';

  return 0;
}
