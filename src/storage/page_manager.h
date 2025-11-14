class PageManager {
public:
    PageManager(std::string collection, uint32_t dimensionality);
    void insert_vector(uint64_t id, const std::vector<float>& vec);
    std::vector<float> get_vector(uint64_t id);
private:
    std::string collection_;
    uint32_t dimensionality_;
    uint32_t next_page_id_;
    std::unordered_map<uint32_t, std::unique_ptr<Page>> pages_;
}