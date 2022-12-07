#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iterator>
#include <memory>
#include <numeric>
#include <functional>

class Directory;
class Object;

using Func = std::function<void(const Object&)>;

class Object {
public:
  Object(Directory* parent) : parent(parent) {}

  virtual ~Object() {}

  virtual bool isDirectory() const = 0;

  virtual size_t size() const = 0;

  virtual void foreach(const Func& f) const = 0;

  Directory* getParent() const {
    return parent;
  }

private:
  Directory* parent;
};

class Directory : public Object {
public:
  Directory(Directory* parent) : Object(parent) {}

  bool isDirectory() const override {
    return true;
  }

  size_t size() const override {
    return std::accumulate(objects.begin(), objects.end(), 0, [](const size_t& a, const auto& b) { return a + b.second->size(); });
  }

  virtual void foreach(const Func& f) const override {
    f(*this);
    for (const auto& [name, object] : objects) {
      object->foreach(f);
    }
  }

  template <class T>
  T* get(const std::string name) const {
    return static_cast<T*>(objects.at(name).get());
  }

  void add(const std::string& name, std::unique_ptr<Object> object) {
    objects[name] = std::move(object);
  }

private:
  std::map<const std::string, std::unique_ptr<Object>> objects;
};

class File : public Object {
public:
  File(Directory* parent, size_t totalSize) : Object(parent), totalSize(totalSize) {}

  bool isDirectory() const override {
    return false;
  }

  size_t size() const override {
    return totalSize;
  }

  virtual void foreach(const Func& f) const override {
    f(*this);
  }

private:
  const size_t totalSize;
};

class FileSystem {
public:
  void cd(const std::string& name) {
    if (name == "/")
      cwd = &root;
    else if (name == "..")
      cwd = cwd->getParent();
    else
      cwd = cwd->get<Directory>(name);
  }

  void mkdir(const std::string& name) {
    if (name == "/")
      return;

    cwd->add(name, std::make_unique<Directory>(cwd));
  }

  void touch(const std::string& name, size_t size) {
    cwd->add(name, std::make_unique<File>(cwd, size));
  }

  void foreach(const Func& f) const {
    root.foreach(f);
  }

  size_t part1() const {
    std::vector<size_t> sizes;
    foreach([&sizes](const Object& o) {
      if (auto size = o.size(); o.isDirectory() && size <= 100000)
        sizes.push_back(size);
    });

    return std::accumulate(sizes.begin(), sizes.end(), 0);
  }

  size_t part2() const {
    size_t best = root.size();
    const size_t needed = root.size() + 30000000 - 70000000;
    foreach([&best, &needed](const Object& o) {
      if (auto size = o.size(); o.isDirectory() && size >= needed && size < best)
        best = size;
    });
    return best;
  }
  
private:
  Directory root{nullptr};
  Directory* cwd = &root;
};

int main() {
  const auto split = [](const std::string& s) -> std::vector<std::string> {
    std::vector<std::string> result;
    std::istringstream ss(s);
    std::copy(std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>(), std::back_inserter(result));
    return result;
  };

  std::vector<std::vector<std::string>> lines;
  for (std::string line; std::getline(std::cin, line);)
    lines.push_back(split(line));

  FileSystem fs;
  for (size_t i = 0; i < lines.size(); i++) {
    const auto& line = lines[i];
    if (line[0] == "$") {
      if (line[1] == "cd") {
        fs.cd(line[2]);
      }
    } else {
      if (line[0] == "dir") {
        fs.mkdir(line[1]);
      } else {
        fs.touch(line[1], std::stoul(line[0]));
      }
    }
  }
  
  std::cout << "Part 1 result = " << fs.part1() << '\n';
  std::cout << "Part 2 result = " << fs.part2() << '\n';
  return 0;
}
