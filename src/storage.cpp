#include "storage.h"
#include <fstream>

Storage::Storage(uint32_t dimension) : _dimension(dimension)
{
}

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

ErrorCode Storage::flush_to_disk(const std::string &filepath)
{
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        return ErrorCode::FileOpenError;
    }

    // write dimension
    file.write(reinterpret_cast<const char*>(&_dimension), sizeof(uint32_t));

    // write number of vectors
    uint32_t num_vectors = _records.size();
    file.write(reinterpret_cast<const char*>(&num_vectors), sizeof(uint32_t));
    
    std::vector<std::pair<std::string, VectorRecord>> vectors;
    for (const auto& pair : _records) {
        vectors.push_back(pair);
    }

    uint32_t data_offset = sizeof(uint32_t) * 2 + MAX_VECTORS * (20 + sizeof(uint32_t));

    // write vector metadata
    uint32_t offset = data_offset;
    for (const auto& [id, _] : vectors)
    {
        char id_buffer[20] = {};
        std::strncpy(id_buffer, id.c_str(), sizeof(id_buffer) - 1);
        file.write(id_buffer, sizeof(id_buffer));

        file.write(reinterpret_cast<const char*>(&offset), sizeof(uint32_t));
        offset += _dimension * sizeof(float);
    }

    // move to data offset and write vector data
    file.seekp(data_offset, std::ios::beg);
    for (const auto& [_, record] : vectors)
    {
        file.write(reinterpret_cast<const char*>(record.values.data()), _dimension * sizeof(float));
    }

    return ErrorCode::None;
}

ErrorCode Storage::load_from_disk(const std::string &filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        return ErrorCode::FileOpenError;
    }

    // read dimension
    uint32_t dimension;
    file.read(reinterpret_cast<char*>(&dimension), sizeof(uint32_t));

    // read number of vectors
    uint32_t num_vectors;
    file.read(reinterpret_cast<char*>(&num_vectors), sizeof(uint32_t));

    // read vector metadata
    std::vector<std::pair<std::string, uint32_t>> metadata;
    for (uint32_t i = 0; i < num_vectors; ++i)
    {
        char id_buffer[20] = {};
        file.read(id_buffer, sizeof(id_buffer));
        
        uint32_t offset;
        file.read(reinterpret_cast<char*>(&offset), sizeof(uint32_t));
        
        metadata.push_back({std::string(id_buffer), offset});
    }

    // read vector data
    std::unordered_map<std::string, VectorRecord> records;
    for (const auto& [id, offset] : metadata)
    {
        file.seekg(offset, std::ios::beg);
        std::vector<float> values(dimension);
        file.read(reinterpret_cast<char*>(values.data()), dimension * sizeof(float));
        
        VectorRecord record{id, values};
        records[id] = record;
    }

    _records = std::move(records);
    return ErrorCode::None;
}