#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

int main() {
  std::string line;
  std::vector<unsigned> elves;
  unsigned calories = 0;

  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      elves.push_back(calories);
      calories = 0;
    } else {
      calories += std::stoi(line);
    }
  }
  elves.push_back(calories);

  std::ranges::sort(elves);

  std::cout << "Part 1 result = " << elves.back() << '\n';
  std::cout << "Part 2 result = " << std::accumulate(std::prev(elves.end(), 3), elves.end(), 0) << '\n';

  return 0;
}
