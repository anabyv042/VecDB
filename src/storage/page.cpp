#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <unordered_map>

/*
Page structure:

HEADER
PageID - 4 bytes
Dimensionality - 4 bytes
Number of vectors in the page - 4 bytes

METADATA
For each vector:
    VectorID - 8 bytes
    Offset - 4 bytes

DATA
Vector data
*/

constexpr uint32_t PAGE_SIZE = 4096; // 4KB page size

class Page
{
public:
    Page(uint32_t page_id, uint32_t dimensionality);
    void insert_vector(std::uint64_t id, const std::vector<float> &vector);
    std::vector<float> get_vector(std::uint64_t id);

    bool has_space() const;
    uint32_t get_page_id();

private:
    uint32_t _page_id;
    uint32_t _dimensionality;
    uint32_t _size_per_vector;

    std::vector<std::byte> _raw_data;
    uint32_t _metadata_offset;
    uint32_t _data_offset;
    std::unordered_map<uint64_t, uint32_t> _offset_map; // maps VectorID to offset in data section

    void write_header();
    uint32_t header_size() const;
};

Page::Page(uint32_t page_id, uint32_t dimensionality)
    : _page_id(page_id), _dimensionality(dimensionality)
{

    // precalculate size per vector entry in metadata + data
    int metadata_size = sizeof(uint64_t) + sizeof(uint32_t);
    _size_per_vector = metadata_size + dimensionality * sizeof(float);

    // create a raw data buffer initialized to zero
    _raw_data.resize(PAGE_SIZE, std::byte{0});

    write_header();

    // calculate how many vectors can fit in the page
    int vectors_per_page = (PAGE_SIZE - header_size()) / _size_per_vector;
    // metadata starts after the header
    _metadata_offset = header_size();
    // calculate max metadata size as number of vectors per page, multiplied by size of each metadata entry
    // each metadata entry is VectorID (8 bytes) + Offset (4 bytes)
    _data_offset = _metadata_offset + vectors_per_page * metadata_size;
}

void Page::insert_vector(std::uint64_t id, const std::vector<float> &vector)
{
    // if (!has_space(id, vector)) {
    //     throw std::runtime_error("Not enough space in page to insert vector");
    // }

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
    // TODO check if id is in map
    uint32_t offset = _offset_map[id];
    std::vector<float> vector(_dimensionality);
    std::memcpy(vector.data(), _raw_data.data() + offset, _dimensionality * sizeof(float));
    return vector;
}

bool Page::has_space() const
{
    return (header_size() + (_offset_map.size() + 1) * _size_per_vector) <= PAGE_SIZE;
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