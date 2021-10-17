#include <fmt/format.h>
#include <fmt/os.h>
#include <fmt/ranges.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <magic_enum.hpp>
#include <optional>
#include <unordered_map>
#include <vector>

const std::string source_path = __FILE__;

//      UP
// LEFT    RIGHT
//     DOWN

enum class Directions { UP, RIGHT, DOWN, LEFT };

// CW
Directions nextDirection(const Directions direction)
{
    return magic_enum::enum_value<Directions>((magic_enum::enum_index(direction).value() + 1)
                                              % magic_enum::enum_count<Directions>());
}

// OCW
Directions previousDirection(const Directions direction)
{
    return magic_enum::enum_value<Directions>((magic_enum::enum_index(direction).value() - 1)
                                              % magic_enum::enum_count<Directions>());
}

struct Distance
{
    int x; // <----> (Columns direction)
    int y; // ^v (Rows direction)
private:
    friend bool operator==(const Distance &lhs, const Distance &rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

// (0, 0) (1, 0) (2, 0) ... (n, 0)
// (0, 1)                     .
//   .                        .
//   .                        .
// (0, n) ................. (n, n)
class Map
{
public:
    using map_t = std::vector<std::vector<char>>;
    using grid_t = map_t::value_type::value_type;
    Map(const size_t row, const size_t col) : map_(row, map_t::value_type(col, FREE_GRID)) {}
    Map(const std::string &filename)
    {
        std::ifstream file(filename.data());

        if (!file.good())
            throw fmt::system_error(errno, "File '{}' doesn't exists", filename);

        for (std::string line; std::getline(file, line);) {
            std::stringstream stream(line);
            map_.emplace_back(std::istream_iterator<grid_t>(stream),
                              std::istream_iterator<grid_t>());
        }
    }
    grid_t getGrid(const size_t x, const size_t y) const { return map_.at(y).at(x); }

    bool isValidGrid(const Distance &pose) const
    {
        if (0 <= pose.x && pose.x < maxX() && 0 <= pose.y && pose.y < maxY()
            && getGrid(pose.x, pose.y) == FREE_GRID)
            return true;
        return false;
    }
    bool isValidMove(const Distance &pose, const Directions direction) const
    {
        int new_x = pose.x + direction_to_distance.at(direction).x;
        int new_y = pose.y + direction_to_distance.at(direction).y;
        return isValidGrid({new_x, new_y});
    }
    Distance move(const Distance &pose, Directions direction) const {
      if (isValidMove(pose, direction)) {
        return {pose.x + direction_to_distance.at(direction).x,
                pose.y + direction_to_distance.at(direction).y};
      }
      return pose;
    }
    void print() const
    {
        for (const auto &row : map_)
            fmt::print("{}\n", fmt::join(row, " "));
    }
    void print(const Distance pose) const
    {
        for (size_t row = 0; row < map_.size(); ++row)
            if (row == pose.y)
                fmt::print("{}{}{} {}\n",
                           fmt::join(map_.at(row).cbegin(),
                                     std::next(map_.at(row).cbegin(), pose.x),
                                     " "),
                           pose.x == 0 ? "" : " ",
                           AGENT_GRID,
                           fmt::join(std::next(map_.at(row).cbegin(), pose.x + 1),
                                     map_.at(row).cend(),
                                     " "));
            else
                fmt::print("{}\n", fmt::join(map_.at(row), " "));
    }
    size_t maxY() const { return map_.size(); }
    size_t maxX() const { return map_.at(0).size(); }

private:
    map_t map_;

    inline static const std::unordered_map<Directions, Distance>
        direction_to_distance{{Directions::UP, {0, -1}},
                              {Directions::DOWN, {0, 1}},
                              {Directions::LEFT, {-1, 0}},
                              {Directions::RIGHT, {1, 0}}};
    inline static const grid_t FREE_GRID = '0';
    inline static const grid_t WALL_GRID = '*';
    inline static const grid_t AGENT_GRID = 'x';
};

class ValueIteration
{
public:
    ValueIteration(const std::shared_ptr<const Map> &map)
        : map_{map}, value_function_(map->maxY(), std::vector<double>(map->maxX(), 0.0)),
          reward_(map->maxY(), std::vector<double>(map->maxX(), 0.0)),
          policy_(map->maxY(), std::vector<Directions>(map->maxX(), Directions::UP))
    {}
    void setReward(const Distance pose, double reward) { reward_.at(pose.y).at(pose.x) = reward; }
    void printMap() const { map_->print(); }
    void printReward() const
    {
        for (const auto &row : reward_)
            fmt::print("{}\n", fmt::join(row, " "));
    }
    void printValueFunction() const {
      for (const auto &row : value_function_)
        fmt::print("{:.2f}\n", fmt::join(row, " "));
    }

    void printPolicy() const {
      for (const auto &row : policy_) {
        for (const auto &col : row)
          fmt::print("{} ", magic_enum::enum_name(col));
        fmt::print("\n");
      }
    }
    double stateTransitionProbability(const Distance current_pose,
                                      const Directions direction,
                                      const Distance next_pose)
    {
      if (next_pose == map_->move(current_pose, direction))
        return 0.8;
      else if (next_pose == map_->move(current_pose, nextDirection(direction)))
        return 0.1;
      else if (next_pose ==
               map_->move(current_pose, previousDirection(direction)))
        return 0.1;
      else
        return 0.0;
    }

    void run(size_t iterations) {
      value_function_ = reward_;
      for (size_t i = 1; i < iterations; ++i) {
        auto value_function = value_function_;
        for (int x = 0; x < map_->maxX(); ++x) {
          for (int y = 0; y < map_->maxY(); ++y) {
            if (reward_.at(y).at(x) != 0.0 || !map_->isValidGrid({x, y}))
              continue;
            double max_value = 0.0;
            for (const Directions action :
                 magic_enum::enum_values<Directions>()) {
              if (!map_->isValidMove({x, y}, action)) continue;
              double value = 0.0;
              for (const Directions direction :
                   {action, previousDirection(action), nextDirection(action)}) {
                const auto next_pose = map_->move({x, y}, direction);
                value +=
                    stateTransitionProbability({x, y}, action, next_pose) *
                    (reward_.at(y).at(x) +
                     lambda * value_function_.at(next_pose.y).at(next_pose.x));
              }
              if (value >= max_value) {
                value_function.at(y).at(x) = value;
                policy_.at(y).at(x) = action;
                max_value = value;
              }
            }
          }
        }
        value_function_ = std::move(value_function);
      }
    }

private:
    std::shared_ptr<const Map> map_;
    std::vector<std::vector<double>> value_function_;
    std::vector<std::vector<double>> reward_;  // map pose -> reward .?
    std::vector<std::vector<Directions>> policy_;
    double lambda = 0.9;
};

int main()
{
    std::filesystem::path file_path(source_path.substr(0, source_path.find_last_of('/') + 1)
                                    + "map.txt");
    Map map(file_path);
    ValueIteration value_iteration(std::make_shared<Map>(file_path));
    value_iteration.setReward({3, 0}, 1);
    value_iteration.setReward({3, 1}, -1);
    value_iteration.run(1000);
    value_iteration.printPolicy();
    return 0;
}
