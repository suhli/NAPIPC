
namespace NIPC {
    class NChar {
    public:
        NChar(char *data, size_t size) : data_(data), size_(size) {};

        char *getData() { return data_; };

        size_t getSize() { return size_; };

    private:
        char *data_;
        size_t size_;
    };
}