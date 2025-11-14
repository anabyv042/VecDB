#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "../error.h"
#include "page.h"

Page::Page(uint32_t page_id, uint32_t dimensionality)
    : _page_id(page_id), _dimensionality(dimensionality)
{
    _size_per_vector = metadata_size() + vector_size();

    // create a raw data buffer initialized to zero
    _raw_data.resize(PAGE_SIZE, std::byte{0});

    write_header();

    // calculate how many vectors can fit in the page
    int vectors_per_page = (PAGE_SIZE - header_size()) / _size_per_vector;

    // metadata starts after the header
    _metadata_offset = header_size();

    // calculate max metadata size as number of vectors per page, multiplied by size of each metadata entry
    // data starts after all metadata
    _data_offset = _metadata_offset + vectors_per_page * metadata_size();
}

Page::Page(const std::string& file_path) {
    std::ifstream ifs(file_path, std::ios::in | std::ios::binary);
    if (!ifs) {
        throw error::FileOpenError(file_path);
    }

    _raw_data.resize(PAGE_SIZE, std::byte{0});
    ifs.read(reinterpret_cast<char*>(_raw_data.data()), _raw_data.size());

    // read header
    std::memcpy(&_page_id, _raw_data.data(), sizeof(_page_id));
    std::memcpy(&_dimensionality, _raw_data.data() + sizeof(_page_id), sizeof(_dimensionality));
    uint32_t vector_count;
    std::memcpy(&vector_count, _raw_data.data() + sizeof(_page_id) + sizeof(_dimensionality), sizeof(vector_count));

    _metadata_offset = header_size();
    for (int i = 0; i < vector_count; ++i) {
        uint64_t id;
        uint32_t offset;
        std::memcpy(&id, _raw_data.data() + _metadata_offset, sizeof(id));
        std::memcpy(&offset, _raw_data.data() + _metadata_offset + sizeof(id), sizeof(offset));
        _metadata_offset += metadata_size();
        _offset_map[id] = offset;
    }

    _size_per_vector = metadata_size() + vector_size();
    int vectors_per_page = (PAGE_SIZE - header_size()) / _size_per_vector;
    _data_offset = _metadata_offset + vectors_per_page * metadata_size();
}

void Page::insert_vector(std::uint64_t id, const std::vector<float> &vector)
{
    if (!has_space())
    {
        throw error::NotEnoughSpace("Not enough space in page to insert vector");
    }

    if (vector.size() != _dimensionality)
    {
        throw error::DimensionalityMismatch(_dimensionality, vector.size());
    }

    // write metadata
    std::memcpy(_raw_data.data() + _metadata_offset, &id, sizeof(id));
    std::memcpy(_raw_data.data() + _metadata_offset + sizeof(id), &_data_offset, sizeof(_data_offset));
    _metadata_offset += sizeof(id) + sizeof(_data_offset);

    // store offset in map
    _offset_map[id] = _data_offset;

    // write vector data
    std::memcpy(_raw_data.data() + _data_offset, vector.data(), _dimensionality * sizeof(float));
    _data_offset += _dimensionality * sizeof(float);

    // rewrite header to increment vector count
    write_header();
}

std::vector<float> Page::get_vector(std::uint64_t id)
{
    if (!_offset_map.contains(id))
    {
        throw error::NotFound("Vector ID not found in page");
    }
    uint32_t offset = _offset_map[id];
    std::vector<float> vector(_dimensionality);
    std::memcpy(vector.data(), _raw_data.data() + offset, _dimensionality * sizeof(float));
    return vector;
}

uint32_t Page::get_page_id()
{
    return _page_id;
}

bool Page::has_space() const
{
    return (header_size() + (_offset_map.size() + 1) * _size_per_vector) <= PAGE_SIZE;
}

void Page::flush_to_disk(const std::string &file_path)
{
    std::ofstream fs(file_path, std::ios::out | std::ios::trunc);
    if (!fs)
    {
        throw error::FileOpenError(file_path);
    }
    fs.write(reinterpret_cast<const char*>(_raw_data.data()), _raw_data.size());
}

void Page::write_header()
{
    int offset = 0;

    // write page ID
    std::memcpy(_raw_data.data(), &_page_id, sizeof(_page_id));
    offset += sizeof(_page_id);

    // write dimensionality
    std::memcpy(_raw_data.data() + offset, &_dimensionality, sizeof(_dimensionality));
    offset += sizeof(_dimensionality);

    // write vector count
    uint32_t vector_count = _offset_map.size();
    std::memcpy(_raw_data.data() + offset, &vector_count, sizeof(vector_count));
}

uint32_t Page::header_size() const
{
    // PageID + Dimensionality + Number of vectors
    return sizeof(_page_id) + sizeof(_dimensionality) + sizeof(uint32_t);
}

uint32_t Page::metadata_size() const
{
    // each metadata entry is VectorID (8 bytes) + Offset (4 bytes)
    return sizeof(uint64_t) + sizeof(uint32_t);
}

uint32_t Page::vector_size() const
{
    return _dimensionality * sizeof(float);
}