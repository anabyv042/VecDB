#include <format>

enum class ErrorCode
{
    None,
    FileOpenError,
};

namespace error
{
    class NotEnoughSpace : public std::runtime_error
    {
    public:
        explicit NotEnoughSpace(const std::string &msg)
            : std::runtime_error(msg) {}
    };

    class DimensionalityMismatch : public std::runtime_error
    {
    public:
        explicit DimensionalityMismatch(int expected, int actual)
            : std::runtime_error(std::format("Invalid vector dimensionality. Expected {}, actual {}", expected, actual)) {}
    };

    class NotFound : public std::runtime_error
    {
        public:
        explicit NotFound(const std::string &msg)
            : std::runtime_error(msg) {}
    };

    class FileOpenError : public std::runtime_error
    {
    public:
        explicit FileOpenError(const std::string &filename)
            : std::runtime_error(std::format("Failed to open file {}", filename)) {}
    };
}