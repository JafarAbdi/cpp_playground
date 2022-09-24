#include <fmt/core.h>

#include <memory>
#include <vector>

struct concept {};

template <typename T>
struct model : concept {
  model(T&& t_) : t(std::forward<T>(t_)) {}
  T t;
};

class asd
{
  private:
    struct asdConcept
    {
      virtual ~asdConcept() = default;
      /* Operations */
      /* clone ???? */
    };
    template <typename T>
    struct asdModel : asdConcept
    {
      asdModel(T&& value): object(std::forward<T>(value)){}
      /* Operations */

      T object;
    };
  /* Operations */
  std::unique_ptr<asdConcept> pimpl;

  public:
    template <typename T>
    asd(const T& x): pimpl(std::make_unique<asdModel<T>>(x)){}
    /* Special member functions */

  };

using objects_t = std::vector<std::unique_ptr<concept>>;
int main(int argc, char* argv[]) {
  objects_t objects;
  objects.emplace_back(std::make_unique<model<int>>(10));
  objects.emplace_back(std::make_unique<model<std::string>>("hola"));
  return 0;
}
