#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <cassert>
#include <numeric>

constexpr std::array<char, 5> DIGITS{'=', '-', '0', '1', '2'};

char toDigit(int i) {
  return DIGITS[i + 2];
}

int fromDigit(char c) {
  for (unsigned i = 0; i < DIGITS.size(); i++)
    if (DIGITS[i] == c)
      return i - 2;
  assert(false);
}

std::string addSNAFU(std::string s1, std::string s2) {
  const auto loan = [](std::string& s, const size_t start) {
    size_t i = start;
    while (i < s.size() && s[--i] == '=')
      ;

    s[i] = toDigit(fromDigit(s[i]) - 1);
    for (size_t j = i + 1; j < start; j++) {
      s[j] = '2';
    }
  };

  std::string result;

  // Pad to equal length
  while (s1.size() < s2.size())
    s1 = "0" + s1;

  while (s2.size() < s1.size())
    s2 = "0" + s2;

  // Add together two SNAFU numbers one digit at at time
  int carry = 0;
  for (size_t i = s1.size() - 1; i < s1.size(); i--) {
    int sum = carry + fromDigit(s1[i]) + fromDigit(s2[i]);
    carry = 0;

    // Result is too small, need to 'loan' value from the left
    if (sum < -2) {
      loan(s1, i);
      sum += 5;
    }

    // Result is too big, need to carry value to the left
    if (sum > 2) {
      sum -= 5;
      carry = 1;
    }
    result = toDigit(sum) + result;
  }

  if (carry)
    result = toDigit(carry) + result;

  return result;
}

using Numbers = std::vector<std::string>;

std::istream& operator>>(std::istream& is, Numbers& numbers) {
  std::string line;

  while (is >> line)
    numbers.push_back(line);

  return is;
}

int main() {
  Numbers numbers;

  std::cin >> numbers;

  std::cout << "Part 1 result = " << std::accumulate(numbers.begin(), numbers.end(), std::string{"0"}, [](const auto& a, const auto& b) { return addSNAFU(a, b); }) << '\n';

  return 0;
}
