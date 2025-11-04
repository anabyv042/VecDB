#include <vector>
#include <string>
#include <optional>
#include "error.h"
#include <unordered_map>

/*
Storage file structure:
4 bytes: dimension of vectors
4 bytes: number of vectors in the file
for each vector:
    20 bytes for ID (padded with null bytes if shorter)
    4 bytes: offset in the file where the vector data starts
for each vector:
    vector data (4 bytes per float)
*/

const size_t MAX_VECTORS = 1000;

struct VectorRecord
{
    std::string id;
    std::vector<float> values;
};

class Storage
{
public:
    Storage(uint32_t dimension);
    void insert(const std::string &id, const std::vector<float> &values);
    std::optional<std::vector<float>> retrieve(const std::string &id) const;
    ErrorCode flush_to_disk(const std::string &filepath);
    ErrorCode load_from_disk(const std::string &filepath);
private:
    uint32_t _dimension;
    std::unordered_map<std::string, VectorRecord> _records;
};