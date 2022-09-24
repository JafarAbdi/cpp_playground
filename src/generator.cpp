#include <fmt/core.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#if __cplusplus > 201703L
using asd = int;
#else
using asd = double;
#endif

auto file_reader(const std::filesystem::path& path) {
  return [file = std::ifstream(path)]() mutable -> std::optional<std::string> {
    while (file.good()) {
      std::string line;
      std::getline(file, line);
      return std::move(line);
    }
    return {};
  };
}

int main(int argc, char** argv) {
  int i = 10;
  std::string s = "asd";
  fmt::print("hola\nhola\n");
  fmt::print("hola:");
  auto asd = file_reader("/home/jafar/workspaces/cpp/playground/CMakeLists.txt");
  for (auto a = asd(); a; a = asd()) fmt::print("{}\n", a.value());
  std::cout << "ad" << std::endl;
}
