#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <memory>
#include <sstream>

class Instruction {
public:
  virtual unsigned cycles() = 0;

  virtual void run(int& reg) = 0;

  virtual ~Instruction() {}

};

class Add : public Instruction {
public:
  Add(int value) : value(value) {}

  unsigned cycles() override {
    return 2;
  }

  void run(int& reg) override {
    reg += value;
  }

private:
  const int value;
};

class Noop : public Instruction {
public:
  unsigned cycles() override {
    return 1;
  }

  void run([[maybe_unused]] int& reg) override { }
};

using Program = std::vector<std::unique_ptr<Instruction>>;

class Device {
public:
  void run(const Program& program) {
    int reg = 1;
    unsigned cycle = 1;
    unsigned measuringCycle = 20;

    strength = 0;
    CRT.clear();

    for (const auto& instruction : program) {
      for (unsigned i = 0; i < instruction->cycles(); i++) {
        // Measuring
        if (cycle == measuringCycle) {
          strength += reg * cycle;
          measuringCycle += WIDTH;
        }

        // Drawing
        const int pos = (cycle - 1) % WIDTH;
        CRT.push_back(pos == reg - 1 || pos == reg || pos == reg + 1 ? '#' : ' ');

        cycle++;
      }
      instruction->run(reg);
    }
  }

  int getStrength() const {
    return strength;
  }

  friend std::ostream& operator<<(std::ostream& os, const Device& device);
private:
  static constexpr size_t WIDTH = 40;

  int strength = 0;
  std::vector<char> CRT;
};

std::ostream& operator<<(std::ostream& os, const Device& device) {
  for (size_t i = 0; i < device.CRT.size(); i++) {
    if (i % device.WIDTH == 0)
      os << '\n';
    os << device.CRT[i];
  }

  return os;
}

int main() {
  std::string instr;
  int value;
  Device device;
  Program program;

  while (std::cin >> instr) {
    if (instr == "noop") {
      program.push_back(std::make_unique<Noop>());
    } else if (instr == "addx") {
      std::cin >> value;
      program.push_back(std::make_unique<Add>(value));
    }
  }

  device.run(program);
  std::cout << "Part 1 result = " << device.getStrength() << '\n';
  std::cout << "Part 2 result = " << device << '\n';

  return 0;
}
