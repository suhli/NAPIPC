#include <cstdio>
namespace NIPC{
    class NChar {
    public:
        NChar(char* data,size_t size);
        char* getData();
        size_t getSize();
    private:
        char* data_;
        size_t size_;
    };
}




