#include "query.h"

Query::Query(Storage s) : _storage(s)
{
}

std::optional<std::vector<float>> Query::get_by_id(const std::string& id)
{
    return _storage.retrieve(id);
}

void Query::insert(const std::string &id, const std::vector<float> &values)
{
    _storage.insert(id, values);
}