#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

class List;
class Value;

class Element {
public:
  virtual ~Element() {}

  virtual bool isValue() const = 0;

  virtual const List* asList() const {
    return nullptr;
  }

  virtual const Value* asValue() const {
    return nullptr;
  }

  friend std::strong_ordering operator<=>(const Element& left, const Element& right);
};

class List : public Element {
public:
  bool isValue() const override {
    return false;
  }

  const List* asList() const override {
    return this;
  }

  size_t size() const {
    return elements.size();
  }

  const Element& operator[](size_t i) const {
    return *elements[i];
  }

  void add(std::unique_ptr<Element> element) {
    elements.push_back(std::move(element));
  }

private:
  std::vector<std::unique_ptr<Element>> elements;
};

class Value : public Element {
public:
  Value(unsigned value) : value(value) {}

  bool isValue() const override {
    return true;
  }

  const Value* asValue() const override {
    return this;
  }

  operator unsigned() const {
    return value;
  }

  List toList() const {
    List result;
    result.add(std::make_unique<Value>(value));
    return result;
  }

private:
  unsigned value;
};

std::unique_ptr<List> parse(const std::string& line, size_t& i) {
  const auto parseValue = [](const std::string& line, size_t& i) -> unsigned {
    unsigned result = 0;
    for (char c = line[i]; c != ',' && c != ']'; c = line[++i]) {
      result = result * 10 + (c - '0');
    }

    return result;
  };

  auto list = std::make_unique<List>();

  while (i < line.size()) {
    const char c = line[i];
    if (c == '[') {
      i++;
      list->add(parse(line, i));
    } else if (c == ']') {
      i++;
      return list;
    } else if (c == ',') {
      i++;
    } else {
      list->add(std::make_unique<Value>(parseValue(line, i)));
    }
  }

  return list;
}

std::unique_ptr<List> parse(const std::string& line) {
  size_t i = 1;
  return parse(line, i);
}

std::strong_ordering operator<=>(const Element& left, const Element& right) {
  if (left.isValue() && right.isValue()) {
    unsigned v1 = *left.asValue();
    unsigned v2 = *right.asValue();
    return v1 <=> v2;
  } else if (left.isValue() && !right.isValue()) {
    return left.asValue()->toList() <=> right;
  } else if (!left.isValue() && right.isValue()) {
    return left <=> right.asValue()->toList();
  } else {
    auto& l1 = *left.asList();
    auto& l2 = *right.asList();
    for (size_t i = 0; i < l1.size() && i < l2.size(); i++) {
      if (auto cmp = l1[i] <=> l2[i]; cmp != 0)
        return cmp;
    }
    return l1.size() <=> l2.size();
  }
}

using Packets = std::vector<std::unique_ptr<List>>;

unsigned part1(const Packets& packets) {
  unsigned result = 0;
  for (size_t i = 0; i < packets.size(); i += 2) {
    if (*packets[i] < *packets[i + 1])
      result += i / 2 + 1;
  }

  return result;
}

unsigned part2(Packets& packets) {
  packets.push_back(parse("[[2]]"));
  auto* divider1 = packets.back().get();
  packets.push_back(parse("[[6]]"));
  auto* divider2 = packets.back().get();

  std::ranges::sort(packets, [](const auto& p1, const auto& p2) { return *p1 < *p2; });

  unsigned result = 1;
  for (size_t i = 0; i < packets.size(); i++)
    if (auto* ptr = packets[i].get(); ptr == divider1 || ptr == divider2)
      result *= i + 1;

  return result;
}

int main() {
  std::string line;
  Packets packets;

  while (std::cin >> line) {
    packets.push_back(parse(line));
  }

  std::cout << "Part 1 result = " << part1(packets) << '\n';
  std::cout << "Part 2 result = " << part2(packets) << '\n';

  return 0;
}
