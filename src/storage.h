#include <vector>
#include <string>
#include <optional>
#include "error.h"
#include <unordered_map>

struct VectorRecord
{
    std::string id;
    std::vector<float> values;
};

class Storage
{
public:
    void insert(const std::string &id, const std::vector<float> &values);
    std::optional<std::vector<float>> retrieve(const std::string &id) const;
private:
    std::unordered_map<std::string, VectorRecord> _records;
};