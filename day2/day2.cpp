#include <iostream>

auto calcScore1(unsigned opponent, unsigned you) {
  unsigned result = you + 1;

  if ((opponent + 1) % 3 == you) {
    result += 6;
  } else if (opponent == you) {
    result += 3;
  }
  return result;
}

auto calcScore2(unsigned opponent, unsigned outcome) {
  char you;
  if (outcome == 0) {
    you = (opponent + 2) % 3;
  } else if (outcome == 1) {
    you = opponent;
  } else if (outcome == 2) {
    you = (opponent + 1) % 3;
  }

  return calcScore1(opponent, you);
}

int main() {
  char opponent, you;

  unsigned part1 = 0;
  unsigned part2 = 0;

  while (std::cin >> opponent >> you) {
    part1 += calcScore1(opponent - 'A', you - 'X'); 
    part2 += calcScore2(opponent - 'A', you - 'X');
  }

  std::cout << "Part 1 result = " << part1 << '\n';
  std::cout << "Part 2 result = " << part2 << '\n';

  return 0;
}
