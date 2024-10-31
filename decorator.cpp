#include <fmt/core.h>
#include <fmt/ranges.h>
#include <memory>
#include <range/v3/all.hpp>

class PlanningRequestAdapter {
public:
  virtual void plan() const = 0;
  virtual void prePlan() const {};
  virtual void postPlan() const {};
  virtual ~PlanningRequestAdapter() = default;
};

class PlanningRequestAdapterInstance : public PlanningRequestAdapter {
public:
  void plan() const override { fmt::print("Planning\n"); }
};

// Interface
class PlanningRequestAdapterDecorator : public PlanningRequestAdapter {
public:
  explicit PlanningRequestAdapterDecorator(
      std::unique_ptr<PlanningRequestAdapter> adapter)
      : adapter_(std::move(adapter)) {}
  [[nodiscard]] PlanningRequestAdapter *decorator() const {
    return adapter_.get();
  }
  void plan() const override {
    if (adapter_) {
      prePlan();
      adapter_->plan();
      postPlan();
    }
  }

private:
  std::unique_ptr<PlanningRequestAdapter> adapter_;
};

class AddTimeParameterization : public PlanningRequestAdapterDecorator {
public:
  AddTimeParameterization(std::unique_ptr<PlanningRequestAdapter> adapter,
                          const double velocity_scaling_factor,
                          const double acceleration_scaling_factor)
      : PlanningRequestAdapterDecorator(std::move(adapter)),
        velocity_scaling_factor_(velocity_scaling_factor),
        acceleration_scaling_factor_(acceleration_scaling_factor) {}
  void postPlan() const override {
    fmt::print("Calling AddTimeParameterization with {} and {}\n",
               velocity_scaling_factor_, acceleration_scaling_factor_);
  }

private:
  double velocity_scaling_factor_;
  double acceleration_scaling_factor_;
};

class ResolveConstraintFrames : public PlanningRequestAdapterDecorator {
public:
  explicit ResolveConstraintFrames(
      std::unique_ptr<PlanningRequestAdapter> adapter)
      : PlanningRequestAdapterDecorator(std::move(adapter)) {}
  void prePlan() const override {
    fmt::print("Calling ResolveConstraintFrames::plan\n");
  }
};

class FixWorkspaceBounds : public PlanningRequestAdapterDecorator {
public:
  explicit FixWorkspaceBounds(std::unique_ptr<PlanningRequestAdapter> adapter)
      : PlanningRequestAdapterDecorator(std::move(adapter)) {}
  void prePlan() const override {
    fmt::print("Calling FixWorkspaceBounds::plan\n");
  }
};

int main() {
  std::unique_ptr<PlanningRequestAdapter> adapter =
      std::make_unique<FixWorkspaceBounds>(
          std::make_unique<ResolveConstraintFrames>(
              std::make_unique<AddTimeParameterization>(
                  std::make_unique<PlanningRequestAdapterInstance>(), 0.2,
                  0.1)));
  adapter->plan();
  fmt::print("asd", 10, 20);
  return 0;
}
