#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <string>
#include <memory>
#include <cmath>

struct Pos {
  int row{};
  int col{};

  auto operator<=>(const Pos&) const = default;

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

struct Player {
  Pos start{};
  Pos pos{};
  unsigned dir{};

  void reset() {
    pos = start;
    dir = 0;
  }
};

using Positions = std::map<Pos, char>;

class Grid {
public:
  Grid(const Positions& positions) : positions(positions) {}

  virtual ~Grid() {}

  virtual void move(Player& player, unsigned steps) const = 0;

protected:
  static constexpr std::array<Pos, 4> OFFSETS{Pos{0, 1}, Pos{1, 0}, Pos{0, -1}, Pos{-1, 0}};
  const std::map<Pos, char>& positions;
};

class Flat : public Grid {
public:
  Flat(const Positions& positions) : Grid(positions) {}

  void move(Player& player, unsigned steps) const override {
    for (unsigned i = 0; i < steps; i++) {
      if (const auto moved = player.pos + OFFSETS[player.dir]; positions.count(moved)) {
        if (positions.at(moved) == '.') {
          player.pos = moved;
        }
      } else {
        const auto& reverse = OFFSETS[(player.dir + 2) % 4];
        Pos wrapped;
        for (wrapped = player.pos; positions.count(wrapped + reverse); wrapped += reverse)
          ;
        if (positions.at(wrapped) != '#')
          player.pos = wrapped;
      }
    }
  }
};

class Cube : public Grid {
public:
  Cube(const Positions& positions) : Grid(positions) {
    int side = std::sqrt(positions.size() / 6);

    struct Border {
      const Pos start1;
      const Pos offset1;
      const unsigned dir1;
      const Pos start2;
      const Pos offset2;
      const unsigned dir2;
    };

#if 1
    // Hardcoded for input file
    const std::array<Border, 7> borders{
      Border{Pos{0, 50}, Pos{1, 0}, 2, Pos{149, 0}, Pos{-1, 0}, 2}, // Left 1 and left 4
      Border{Pos{50, 50}, Pos{1, 0}, 2, Pos{100, 0}, Pos{0, 1}, 3}, // Left 3 and top 4
      Border{Pos{150, 0}, Pos{1, 0}, 2, Pos{0, 50}, Pos{0, 1}, 3}, // Left 6 and top 1
      Border{Pos{199, 0}, Pos{0, 1}, 1, Pos{0, 100}, Pos{0, 1}, 3}, // Bottom 6 and top 2
      Border{Pos{150, 49}, Pos{1, 0}, 0, Pos{149, 50}, Pos{0, 1}, 1}, // Right 6 and bottom 5
      Border{Pos{100, 99}, Pos{1, 0}, 0, Pos{49, 149}, Pos{-1, 0}, 0}, // Right 5 and right 2
      Border{Pos{50, 99}, Pos{1, 0}, 0, Pos{49, 100}, Pos{0, 1}, 1}, // Right 3 and bottom 2
    };
#else
    // Hardcoded for test file
    const std::array<Border, 7> borders{
      Border{Pos{0, 8}, Pos{1, 0}, 2, Pos{4, 4}, Pos{0, 1}, 3}, // Left 1 and top 3
      Border{Pos{4, 0}, Pos{0, 1}, 3, Pos{0, 11}, Pos{0, -1}, 3}, // Top 2 and top 1
      Border{Pos{0, 4}, Pos{1, 0}, 2, Pos{11, 15}, Pos{0, -1}, 1}, // Left 2 and bottom 6
      Border{Pos{7, 0}, Pos{0, 1}, 1, Pos{11, 11}, Pos{0, -1}, 1}, // Bottom 2 and bottom 5
      Border{Pos{7, 4}, Pos{0, 1}, 1, Pos{11, 8}, Pos{-1, 0}, 2}, // Bottom 3 and left 5
      Border{Pos{8, 15}, Pos{1, 0}, 0, Pos{3, 11}, Pos{-1, 0}, 0}, // Right 6 and right 1
      Border{Pos{8, 12}, Pos{0, 1}, 3, Pos{7, 11}, Pos{-1, 0}, 0}, // Top 6 and right 4
    };
#endif

    for (const auto& border : borders) {
      Pos p1 = border.start1;
      Pos p2 = border.start2;
      for (int i = 0; i < side; i++) {
        PosDir pd1{p1.row, p1.col, border.dir1};
        PosDir pd2{p2.row, p2.col, border.dir2};
        teleport[pd1] = pd2;
        teleport[pd2] = pd1;
        p1 += border.offset1;
        p2 += border.offset2;
      }
    }
  }

  void move(Player& player, unsigned steps) const override {
    for (unsigned i = 0; i < steps; i++) {
      if (const auto moved = player.pos + OFFSETS[player.dir]; positions.count(moved)) {
        if (positions.at(moved) == '.') {
          player.pos = moved;
        }
      } else { // Teleport
        const auto teleported = teleport.at({player.pos.row, player.pos.col, player.dir});
        if (positions.at({teleported.row, teleported.col}) != '#') {
          player.pos = {teleported.row, teleported.col};
          player.dir = (teleported.dir + 2) % 4;
        }
      }
    }
  }

private:
  struct PosDir {
    int row{};
    int col{};
    unsigned dir{};

    auto operator<=>(const PosDir&) const = default;
  };
  std::map<PosDir, PosDir> teleport;
};

class Instr {
public:
  virtual void run(Player& player, const Grid& grid) const = 0;

  virtual ~Instr() {}
};

class RotInstr : public Instr {
public:
  RotInstr(char rot) : rot(rot) {}

  void run(Player& player, [[maybe_unused]] const Grid& grid) const override {
    if (rot == 'R') {
      player.dir = (player.dir + 1) % 4;
    } else {
      player.dir = (player.dir + 3) % 4;
    }
  }

private:
  const char rot;
};

class MoveInstr : public Instr {
public:
  MoveInstr(unsigned steps) : steps(steps) {}

  void run(Player& player, const Grid& grid) const override {
    grid.move(player, steps);
  }

private:
  const unsigned steps;
};

class Board {
public:
  unsigned run(bool part1) {
    Flat flat{positions};
    Cube cube{positions};

    player.reset();
    for (const auto& instr : instructions)
      if (part1)
        instr->run(player, flat);
      else
        instr->run(player, cube);

    return 1000 * (player.pos.row + 1) + 4 * (player.pos.col + 1) + player.dir;
  }

  friend std::istream& operator>>(std::istream& is, Board& board);

private:
  Positions positions;
  Player player;
  std::vector<std::unique_ptr<Instr>> instructions;
};

std::istream& operator>>(std::istream& is, Board& board) {
  std::string line;

  unsigned row = 0;
  int startCol = -1;
  while (std::getline(is, line)) {
    if (!(line.front() == ' ' || line.front() == '.' || line.front() == '#'))
      break;

    for (unsigned col = 0; col < line.size(); col++) {
      if (line[col] != ' ') {
        if (startCol == -1) {
          startCol = col;
        }
        board.positions[Pos(row, col)] = line[col];
      }
    }
    row++;
  }

  board.player.start = {0, startCol};

  unsigned steps;
  char dir;
  while (true) {
    if (is >> steps) {
      board.instructions.push_back(std::make_unique<MoveInstr>(steps));
    } else {
      break;
    }

    if (is >> dir) {
      board.instructions.push_back(std::make_unique<RotInstr>(dir));
    } else {
      break;
    }
  }

  return is;
}

int main() {
  Board board;

  std::cin >> board;

  std::cout << "Part 1 result = " << board.run(true) << '\n';
  std::cout << "Part 2 result = " << board.run(false) << '\n';

  return 0;
}
