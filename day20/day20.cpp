#include <iostream>
#include <vector>
#include <list>
#include <iterator>

struct Coordinate {
  int64_t value{};
  unsigned id{};
  bool mixed{};
};

using File = std::list<Coordinate>;

std::istream& operator>>(std::istream& is, File& file) {
  int64_t value;
  unsigned id = 0;
  while (is >> value)
    file.emplace_back(value, id++, false);
  return is;
}

int64_t mix(File file, unsigned rounds = 1, int64_t mul = 1) {

  for (auto& c : file)
    c.value *= mul;

  int len = file.size();

  for (unsigned round = 0; round < rounds; round++) {
    for (auto& c : file)
      c.mixed = false;

    for (size_t i = 0; i < file.size(); i++) {
      auto current = file.begin();
      while (current->id != i)
        current++;
  
      if (current->mixed) {
        current++;
        continue;
      }
      current->mixed = true;
      
      int64_t value = current->value % (len - 1);
      if (value < 0)
        value += len - 1;

      auto moving = *current;
      auto next = std::next(current);
      file.erase(current);
      current = next;

      auto moved = current;
      for (int64_t i = 0; i < value; i++) {
        if (moved == file.end())
          moved = file.begin();
        moved++;
      }
      file.insert(moved, moving);
    }
  }

  int64_t result = 0;

  for (auto current = file.begin(); current != file.end(); current++) {
    if (current->value == 0) {
      for (size_t i = 1; i <= 3000; i++) {
        current++;
        if (current == file.end())
          current = file.begin();
        if (i % 1000 == 0) {
          result += current->value;
        }
      }
      break;
    }
  }

  return result;
}

int main() {
  File file;
  std::cin >> file;

  std::cout << "Part 1 result = " << mix(file) << '\n';
  std::cout << "Part 2 result = " << mix(file, 10, 811589153) << '\n';

  return 0;
}
