#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "../error.h"
#include "page.h"

Page::Page(uint32_t page_id, uint32_t dimensionality)
    : page_id_(page_id), dimensionality_(dimensionality)
{
    size_per_vector_ = metadata_size() + vector_size();

    // create a raw data buffer initialized to zero
    raw_data_.resize(PAGE_SIZE, std::byte{0});

    write_header();

    // calculate how many vectors can fit in the page
    int vectors_per_page = (PAGE_SIZE - header_size()) / size_per_vector_;

    // metadata starts after the header
    metadata_offset_ = header_size();

    // calculate max metadata size as number of vectors per page, multiplied by size of each metadata entry
    // data starts after all metadata
    data_offset_ = metadata_offset_ + vectors_per_page * metadata_size();
}

Page::Page(const std::string& file_path) {
    std::ifstream ifs(file_path, std::ios::in | std::ios::binary);
    if (!ifs) {
        throw error::FileOpenError(file_path);
    }

    raw_data_.resize(PAGE_SIZE, std::byte{0});
    ifs.read(reinterpret_cast<char*>(raw_data_.data()), raw_data_.size());

    // read header
    std::memcpy(&page_id_, raw_data_.data(), sizeof(page_id_));
    std::memcpy(&dimensionality_, raw_data_.data() + sizeof(page_id_), sizeof(dimensionality_));
    uint32_t vector_count;
    std::memcpy(&vector_count, raw_data_.data() + sizeof(page_id_) + sizeof(dimensionality_), sizeof(vector_count));

    metadata_offset_ = header_size();
    for (int i = 0; i < vector_count; ++i) {
        uint64_t id;
        uint32_t offset;
        std::memcpy(&id, raw_data_.data() + metadata_offset_, sizeof(id));
        std::memcpy(&offset, raw_data_.data() + metadata_offset_ + sizeof(id), sizeof(offset));
        metadata_offset_ += metadata_size();
        offset_map_[id] = offset;
    }

    size_per_vector_ = metadata_size() + vector_size();
    int vectors_per_page = (PAGE_SIZE - header_size()) / size_per_vector_;
    data_offset_ = metadata_offset_ + vectors_per_page * metadata_size();
}

void Page::insert_vector(std::uint64_t id, const std::vector<float> &vector)
{
    if (!has_space())
    {
        throw error::NotEnoughSpace("Not enough space in page to insert vector");
    }

    if (vector.size() != dimensionality_)
    {
        throw error::DimensionalityMismatch(dimensionality_, vector.size());
    }

    // write metadata
    std::memcpy(raw_data_.data() + metadata_offset_, &id, sizeof(id));
    std::memcpy(raw_data_.data() + metadata_offset_ + sizeof(id), &data_offset_, sizeof(data_offset_));
    metadata_offset_ += sizeof(id) + sizeof(data_offset_);

    // store offset in map
    offset_map_[id] = data_offset_;

    // write vector data
    std::memcpy(raw_data_.data() + data_offset_, vector.data(), dimensionality_ * sizeof(float));
    data_offset_ += dimensionality_ * sizeof(float);

    // rewrite header to increment vector count
    write_header();
}

std::vector<float> Page::get_vector(std::uint64_t id)
{
    if (!offset_map_.contains(id))
    {
        throw error::NotFound("Vector ID not found in page");
    }
    uint32_t offset = offset_map_[id];
    std::vector<float> vector(dimensionality_);
    std::memcpy(vector.data(), raw_data_.data() + offset, dimensionality_ * sizeof(float));
    return vector;
}

uint32_t Page::get_page_id()
{
    return page_id_;
}

bool Page::has_space() const
{
    return (header_size() + (offset_map_.size() + 1) * size_per_vector_) <= PAGE_SIZE;
}

void Page::flush_to_disk(const std::string &file_path)
{
    std::ofstream fs(file_path, std::ios::out | std::ios::trunc);
    if (!fs)
    {
        throw error::FileOpenError(file_path);
    }
    fs.write(reinterpret_cast<const char*>(raw_data_.data()), raw_data_.size());
}

void Page::write_header()
{
    int offset = 0;

    // write page ID
    std::memcpy(raw_data_.data(), &page_id_, sizeof(page_id_));
    offset += sizeof(page_id_);

    // write dimensionality
    std::memcpy(raw_data_.data() + offset, &dimensionality_, sizeof(dimensionality_));
    offset += sizeof(dimensionality_);

    // write vector count
    uint32_t vector_count = offset_map_.size();
    std::memcpy(raw_data_.data() + offset, &vector_count, sizeof(vector_count));
}

uint32_t Page::header_size() const
{
    // PageID + Dimensionality + Number of vectors
    return sizeof(page_id_) + sizeof(dimensionality_) + sizeof(uint32_t);
}

uint32_t Page::metadata_size() const
{
    // each metadata entry is VectorID (8 bytes) + Offset (4 bytes)
    return sizeof(uint64_t) + sizeof(uint32_t);
}

uint32_t Page::vector_size() const
{
    return dimensionality_ * sizeof(float);
}