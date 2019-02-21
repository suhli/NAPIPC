#include <string>

struct IPCPacket{
    size_t content_len;
    char content[];
};
