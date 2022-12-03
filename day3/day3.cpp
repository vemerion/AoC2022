#include <iostream>
#include <string>
#include <algorithm>

using Rucksacks = std::vector<std::string>;

unsigned priority(char c) {
  return c < 'a' ? c - 'A' + 27 : c - 'a' + 1;
}

unsigned part1(const Rucksacks& rucksacks) {
  unsigned result = 0;

  for (std::string rucksack : rucksacks) {
    const auto middle = std::next(rucksack.begin(), rucksack.size() / 2);
    std::sort(rucksack.begin(), middle);
    std::sort(middle, rucksack.end());

    std::string shared;
    std::set_intersection(rucksack.begin(), middle, middle, rucksack.end(), std::back_inserter(shared));
    result += priority(shared.front());
  }

  return result;
}


unsigned part2(const Rucksacks& rucksacks) {
  static constexpr unsigned GROUP_SIZE = 3;

  unsigned result = 0;

  for (auto itr = rucksacks.begin(); itr != rucksacks.end(); itr++) {
    std::string shared = *itr;
    std::sort(shared.begin(), shared.end());
    for (unsigned i = 0; i < GROUP_SIZE - 1; i++) {
      std::string rucksack = *(++itr);
      std::sort(rucksack.begin(), rucksack.end());
      std::string intersection;
      std::set_intersection(shared.begin(), shared.end(), rucksack.begin(), rucksack.end(), std::back_inserter(intersection));
      shared = std::move(intersection);
    }
    result += priority(shared.front());
  }

  return result;
}

int main() {
  std::string line;
  Rucksacks rucksacks;

  while (std::cin >> line)
    rucksacks.push_back(line);

  std::cout << "Part 1 result = " << part1(rucksacks) << '\n';
  std::cout << "Part 2 result = " << part2(rucksacks) << '\n';

  return 0;
}
