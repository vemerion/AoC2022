#include <iostream>
#include <array>
#include <set>
#include <vector>
#include <algorithm>
#include <string>
#include <map>

struct Pos {
  int64_t x{};
  int64_t y{};

  auto operator<=>(const Pos&) const = default;

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

using Chamber = std::set<Pos>;

class Rock {
public:
  Rock(std::vector<Pos> shape) : shape(std::move(shape)) {}

  bool collide(const Chamber& chamber, const Pos& offset) const {
    for (const auto& pos : shape) {
      if (auto p = offset + pos; p.x < 0 || p.x > 6 || p.y < 0 || chamber.count(p))
        return true;
    }

    return false;
  }

  void place(Chamber& chamber, const Pos& offset) const {
    for (const auto& pos : shape)
      chamber.insert(offset + pos);
  }

  int64_t top() const {
    return std::ranges::max_element(shape, [](const auto& a, const auto& b) { return a.y < b.y; })->y;
  }

private:
  std::vector<Pos> shape;
};

static const std::array<Rock, 5> ROCKS{Rock{{Pos{0, 0}, Pos{1, 0}, Pos{2, 0}, Pos{3, 0}}}, 
                                       Rock{{Pos{1, 0}, Pos{0, 1}, Pos{1, 1}, Pos{2, 1}, Pos{1, 2}}}, 
                                       Rock{{Pos{0, 0}, Pos{1, 0}, Pos{2, 0}, Pos{2, 1}, Pos{2, 2}}},
                                       Rock{{Pos{0, 0}, Pos{0, 1}, Pos{0, 2}, Pos{0, 3}}},
                                       Rock{{Pos{0, 0}, Pos{1, 0}, Pos{0, 1}, Pos{1, 1}}}};

int64_t fall(const std::string& jet, uint64_t max) {
  Chamber chamber;

  struct State {
    State(uint64_t index, uint64_t step, uint64_t rock, const Chamber& chamber, int64_t height) : index(index) {
      snapshot = std::to_string(step) + "," + std::to_string(rock) + "\n";

      // Assume checking 50 rows is enough
      for (auto y = height; y >= height - 50; y--) {
        for (int64_t x = 0; x < 7; x++) {
          snapshot += chamber.count({x, y}) ? "#" : ".";
        }
        snapshot += "\n";
      }
    }

    const uint64_t index{};
    std::string snapshot;

    auto operator<=>(const State& other) const {
      return snapshot <=> other.snapshot;
    }
  };

  std::set<State> states;
  std::vector<int64_t> heights{};

  const auto calcHeight = [max](const State& first, const State& last, const std::vector<int64_t>& heights) -> int64_t {
    int64_t height = heights[first.index];

    const int64_t heightDiff = heights[last.index] - height;

    const uint64_t period = last.index - first.index;
    const uint64_t periods = (max - 1 - first.index) / period;

    height += periods * heightDiff;

    uint64_t leftover = max - 1 - first.index - periods * period;

    for (uint64_t i = 0; i < leftover; i++) {
      height += heights[first.index + i + 1] - heights[first.index + i];
    }

    return height;
  };

  uint64_t step = 0;
  uint64_t count = 0;
  int64_t height = 0;
  for (uint64_t i = 0; count < max; i++) {
    const Rock& rock = ROCKS[i % ROCKS.size()];
    Pos pos = {2, height + 3};
    
    while (true) {
      step++;
      Pos move = pos + Pos{jet[(step - 1) % jet.size()] == '>' ? 1 : -1, 0};
      if (!rock.collide(chamber, move))
        pos = move;

      move = pos + Pos{0, -1};
      if (!rock.collide(chamber, move)) {
        pos = move;
      } else {
        rock.place(chamber, pos);
        count++;
        height = std::max(height, pos.y + rock.top() + 1);
        heights.push_back(height);
        
        // Check if pattern is found
        auto state = State(i, step % jet.size(), (i + 1) % ROCKS.size(), chamber, height);
        if (auto found = states.find(state); found != states.end()) {
          return calcHeight(*found, state, heights);
        }
        states.insert(state);
        break;
      }
    }
  }

  return height;
}

int main() {
  std::string jet;

  std::cin >> jet;

  std::cout << "Part 1 result = " << fall(jet, 2022) << '\n';
  std::cout << "Part 2 result = " << fall(jet, 1000000000000) << '\n';
  return 0;
}
