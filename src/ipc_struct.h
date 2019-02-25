#include <string>

struct SendInfo{
    NIPC::NChar channel;
    NIPC::NChar value;
};
struct IPCPacket{
    size_t content_len;
    char content[];
};

struct AsyncWorker{
    SendInfo *info;
    napi_ref cb;
    napi_async_work worker;
};
