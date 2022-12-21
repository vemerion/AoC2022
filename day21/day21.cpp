#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <optional>
#include <iterator>
#include <cassert>

class Monkey;

using Monkeys = std::map<std::string, Monkey>;

class Monkey {
public:
  Monkey(const std::string& name, int64_t value) : name(name), value(value) {}
  Monkey(const std::string& name, const std::string& lhs, const std::string& rhs, char op) : name(name), lhs(lhs), rhs(rhs), op(op) {}

  int64_t evaluate(const Monkeys& monkeys) const {
    if (value)
      return *value;

    auto left = monkeys.at(lhs).evaluate(monkeys);
    auto right = monkeys.at(rhs).evaluate(monkeys);

    switch (op) {
      case '+':
        return left + right;
      case '-':
        return left - right;
      case '*':
        return left * right;
      case '/':
        return left / right;
    }

    assert(false);
  }

  int64_t human(const Monkeys& monkeys) const {
    bool humanInLeft = hasHumanInLeft(monkeys);
    int64_t target = monkeys.at(humanInLeft ? rhs : lhs).evaluate(monkeys);
    return monkeys.at(humanInLeft ? lhs : rhs).human(monkeys, target);
  }

private:
  bool hasHumanInLeft(const Monkeys& monkeys) const {
    return monkeys.at(lhs).hasHuman(monkeys);
  }

  int64_t human(const Monkeys& monkeys, int64_t target) const {
    if (name == "humn")
      return target;

    bool humanInLeft = hasHumanInLeft(monkeys);
    int64_t value = monkeys.at(humanInLeft ? rhs : lhs).evaluate(monkeys);
    const auto& monkey = humanInLeft ? monkeys.at(lhs) : monkeys.at(rhs);
    switch (op) {
      case '+':
        return monkey.human(monkeys, target - value);
      case '-':
        return monkey.human(monkeys, humanInLeft ? target + value : value - target);
      case '*':
        return monkey.human(monkeys, target / value);
      case '/':
        return monkey.human(monkeys, humanInLeft ? target * value : value / target);
    }

    assert(false);
  }
  
  bool hasHuman(const Monkeys& monkeys) const {
    if (name == "humn")
      return true;

    if (*value)
      return false;

    return monkeys.at(lhs).hasHuman(monkeys) || monkeys.at(rhs).hasHuman(monkeys);
  }

  const std::string name;
  const std::optional<int64_t> value{};
  const std::string lhs;
  const std::string rhs;
  const char op{};
};


std::istream& operator>>(std::istream& is, Monkeys& monkeys) {
  const auto split = [](const std::string& s) -> std::vector<std::string> {
    std::vector<std::string> result;
    std::istringstream ss(s);
    std::copy(std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>(), std::back_inserter(result));
    return result;
  };

  std::string line;

  while (std::getline(is, line)) {
    auto words = split(line);
    auto name = words[0].substr(0, words[0].size() - 1);
    if (words.size() == 2) {
      monkeys.emplace(name, Monkey{name, std::stoi(words[1])});
    } else {
      monkeys.emplace(name, Monkey{name, words[1], words[3], words[2][0]});
    }
  }

  return is;
}

int main() {
  Monkeys monkeys;

  std::cin >> monkeys;

  std::cout << "Part 1 result = " << monkeys.at("root").evaluate(monkeys) << '\n';
  std::cout << "Part 2 result = " << monkeys.at("root").human(monkeys) << '\n';

  return 0;
}
