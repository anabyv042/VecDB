constexpr uint32_t PAGE_SIZE = 4096; // 4KB page size

class Page
{
public:
    Page(uint32_t page_id, uint32_t dimensionality);
    Page(const std::string &file_path); // load from disk
    void insert_vector(std::uint64_t id, const std::vector<float> &vector);
    std::vector<float> get_vector(std::uint64_t id);
    bool has_space() const;
    uint32_t get_page_id();
    void flush_to_disk(const std::string &file_path);

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
    uint32_t metadata_size() const;
    uint32_t vector_size() const;
};
