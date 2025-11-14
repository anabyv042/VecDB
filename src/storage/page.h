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
    uint32_t page_id_;
    uint32_t dimensionality_;
    uint32_t size_per_vector_;

    std::vector<std::byte> raw_data_;
    uint32_t metadata_offset_;
    uint32_t data_offset_;
    std::unordered_map<uint64_t, uint32_t> offset_map_; // maps VectorID to offset in data section

    void write_header();
    uint32_t header_size() const;
    uint32_t metadata_size() const;
    uint32_t vector_size() const;
};
