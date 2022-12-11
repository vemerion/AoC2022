#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <functional>
#include <algorithm>

using Operation = std::function<unsigned(unsigned)>;

class Monkey {
public:
  void round(std::vector<Monkey>& monkeys, bool part1) {
    for (auto& item : items) {
      item.update(monkeys, operation, part1);
      monkeys[item[id] % test == 0 ? throwTrue : throwFalse].receive(item);
      inspectionCount++;
    }
    items.clear();
  }

  unsigned getInspectionCount() const {
    return inspectionCount;
  }

  unsigned getTest() const {
    return test;
  }

  friend std::istream& operator>>(std::istream& is, Monkey& monkey);

private:
  class Item {
  public:
    Item(unsigned start) {
      values.push_back(start);
    }

    unsigned operator[](unsigned id) const {
      return values[id];
    }

    void update(const std::vector<Monkey>& monkeys, const Operation& op, bool part1) {
      while (values.size() < monkeys.size())
        values.push_back(values.front());

      for (unsigned i = 0; i < monkeys.size(); i++) {
        auto& value = values[i];
        value = op(value);
        if (part1)
          value /= 3;
        else
          value %= monkeys[i].getTest();
      }
    }

  private:
    std::vector<unsigned> values;
  };

  void receive(const Item& item) {
    items.push_back(item);
  }

  unsigned id{};
  std::list<Item> items;
  Operation operation = nullptr;
  unsigned test{};
  unsigned throwTrue{};
  unsigned throwFalse{};
  unsigned inspectionCount{};
};

std::istream& operator>>(std::istream& is, Monkey& monkey) {
  const auto skip = [&is](unsigned count) {
    std::string unused;
    for (unsigned i = 0; i < count; i++)
      is >> unused;
  };

  std::string unused;

  // Id
  is >> unused >> monkey.id >> unused;

  // Items
  skip(2);
  while (true) {
    unsigned item;
    std::string separator;
    is >> item >> separator;
    monkey.items.emplace_back(item);
    if (separator != ",")
      break;
  }

  // Operation
  const auto operand = [](const std::string& s) -> Operation {
    if (s == "old") {
      return [](unsigned worry) { return worry; };
    } else {
      unsigned value = std::stoul(s);
      return [value]([[maybe_unused]] unsigned worry) { return value; };
    }
  };

  std::string left, right;
  char op;
  skip(2);
  is >> left >> op >> right;
  monkey.operation = [lhs = operand(left), rhs = operand(right), op](unsigned worry) -> unsigned {
    return op == '+' ? lhs(worry) + rhs(worry) : lhs(worry) * rhs(worry);
  };

  // Test
  skip(3);
  is >> monkey.test;

  // ThrowTrue
  skip(5);
  is >> monkey.throwTrue;

  // ThrowFalse
  skip(5);
  is >> monkey.throwFalse;

  return is;
}

class Monkeys {
public:
  uint64_t turns(unsigned count, bool part1) {
    auto copy = monkeys;
    for (unsigned i = 0; i < count; i++)
      for (auto& monkey : copy)
        monkey.round(copy, part1);

    std::vector<uint64_t> inspectionCounts;
    for (const auto& monkey : copy) {
      inspectionCounts.push_back(monkey.getInspectionCount());
    }
    std::ranges::sort(inspectionCounts);
    return inspectionCounts[inspectionCounts.size() - 1] * inspectionCounts[inspectionCounts.size() - 2];
  }
  
  friend std::istream& operator>>(std::istream& is, Monkeys& monkeys);

private:
  std::vector<Monkey> monkeys;
};

std::istream& operator>>(std::istream& is, Monkeys& monkeys) {
  std::string line;

  while (true) {
    Monkey monkey;
    std::cin >> monkey;
    monkeys.monkeys.push_back(monkey);
    while (std::cin.peek() == '\n')
      std::cin.ignore();

    if (std::cin.eof())
      break;
  }

  return is;
}

int main() {
  Monkeys monkeys;

  std::cin >> monkeys;

  std::cout << "Part 1 result = " << monkeys.turns(20, true) << '\n';
  std::cout << "Part 2 result = " << monkeys.turns(10000, false) << '\n';
  return 0;
}
