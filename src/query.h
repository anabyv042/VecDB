#include <vector>
#include <string>
#include "storage.h"

class Query {
public:
    void insert(const std::string& id, const std::vector<float>& values);
    std::optional<std::vector<float>> get_by_id(const std::string& id);
    std::vector<std::vector<float>> get_closest();
private:
    Storage _storage;
};