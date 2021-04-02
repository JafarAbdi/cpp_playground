#include <optional>
#include <filesystem>
#include <fstream>
#include <fmt/core.h>

auto file_reader(const std::filesystem::path &path) {
    return [file = std::ifstream(path)]() mutable -> std::optional<std::string> {
        while (file.good()) {
            std::string line;
            std::getline(file, line);
            return std::move(line);
        }
        return {};
    };
}

int main(int argc, char **argv) {
    auto file = file_reader("CMakeLists.txt");
    for (auto line = file(); line; line = file()) {
        fmt::print("{}--\n", line.value());
    }
}