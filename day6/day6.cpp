#include <iostream>
#include <string>
#include <set>

size_t startOf(const std::string& datastream, size_t length) {
  for (size_t i = length; i < datastream.size(); i++) {
    std::set<char> previous(datastream.begin() + i - length, datastream.begin() + i);
    if (previous.size() == length)
      return i;
  }
  return 0;
}

int main() {
  std::string datastream;
  std::cin >> datastream;

  std::cout << "Part 1 result = " << startOf(datastream, 4) << '\n';
  std::cout << "Part 2 result = " << startOf(datastream, 14) << '\n';
  return 0;
}
