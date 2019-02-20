#include "NChar.h"
namespace NIPC{
    NChar::NChar(char *data, size_t size) {
        data_ = data;
        size_ = size;
    }

    char* NChar::getData() {
        return data_;
    }

    size_t NChar::getSize() {
        return size_;
    }
}
