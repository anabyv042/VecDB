#include "storage.h"

void Storage::insert(const std::string &id, const std::vector<float> &values)
{
    VectorRecord record{id, values};
    _records[id] = record;
}

std::optional<std::vector<float>> Storage::retrieve(const std::string &id) const
{
    if (_records.contains(id))
    {
        return _records.at(id).values;
    }
    return std::nullopt;
}