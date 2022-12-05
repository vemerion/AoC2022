#include <iostream>
#include <stack>
#include <vector>
#include <list>
#include <string>

struct Instruction {
  unsigned count = 0;
  unsigned from = 0;
  unsigned to = 0;
};

class Cargo {
public:
  static Cargo parse() {
    Cargo cargo;

    // Stacks
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(std::cin, line) && !line.empty()) {
      lines.push_back(line);
    }

    for (size_t i = 0; i < lines.back().size(); i++) {
      if (lines.back()[i] == ' ')
        continue;

      cargo.stacks.emplace_back();
      for (int j = lines.size() - 2; j >= 0 && lines[j][i] != ' '; j--) {
        cargo.stacks.back().push(lines[j][i]);
      }
    }

    // Instructions
    Instruction instr;
    std::string unused;
    while (std::cin >> unused >> instr.count >> unused >> instr.from >> unused >> instr.to)
      cargo.instructions.push_back(instr);
    return cargo;
  }

  std::string run(bool part2) const {
    std::list<char> popped;
    auto copy = stacks;

    const auto pop = [&popped, &copy, part2](unsigned index) {
      char c = copy[index].top();
      if (part2)
        popped.push_front(c);
      else
        popped.push_back(c);
      copy[index].pop();
    };

    for (const auto& instr: instructions) {
      popped.clear();
      for (unsigned i = 0; i < instr.count; i++) {
        pop(instr.from - 1);
      }
      for (auto c : popped)
        copy[instr.to - 1].push(c);
    }

    std::string result;
    for (const auto& stack : copy) {
      result += stack.top();
    }
    return result;
  }

private:
  std::vector<std::stack<char>> stacks;
  std::vector<Instruction> instructions;
};

int main() {
  Cargo cargo = Cargo::parse();

  std::cout << "Part 1 result = " << cargo.run(false) << '\n';
  std::cout << "Part 2 result = " << cargo.run(true) << '\n';
  return 0;
}
