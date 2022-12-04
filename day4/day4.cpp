#include <iostream>

int main() {
  unsigned min1, max1, min2, max2;
  char unused;

  unsigned part1 = 0;
  unsigned part2 = 0;

  while (std::cin >> min1 >> unused >> max1 >> unused >> min2 >> unused >> max2) {
    if ((min1 >= min2 && max1 <= max2) || (min2 >= min1 && max2 <= max1))
      part1++;
    if ((max1 >= min2 && min1 <= max2) || (max2 >= min1 && min2 <= max1))
      part2++;
  }

  std::cout << "Part 1 result = " << part1 << '\n';
  std::cout << "Part 2 result = " << part2 << '\n';

  return 0;
}
