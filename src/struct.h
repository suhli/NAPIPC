#include <string>

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

    class SendRequest {
    public:
        std::string getChannel() {
            return channel;
        }

        std::string getValue() {
            return value;
        }

        SendRequest(std::string channel, std::string value) : channel(channel), value(value) {};
    private:
        std::string channel;
        std::string value;
    };

    class IPCPacket {
    public:
        void setSize(size_t s) {
            size = s;
        };

        void setContent(std::string c) {
            content = c;
        };

        std::string getContent() {
            return content;
        };

        size_t getSize() {
            return size;
        };

        IPCPacket(size_t content_len, std::string content) : size(content_len), content(content) {};
    private:
        size_t size;
        std::string content;
    };


    class AsyncWorker {
    public:
        SendRequest getInfo() {
            return info;
        };

        napi_ref getCb() {
            return cb;
        };

        napi_async_work getWorker() {
            return worker;
        };

        void setCb(napi_ref c) {
            cb = c;
        };

        void setWorker(napi_async_work w) {
            worker = w;
        };

        AsyncWorker(SendRequest info) : info(info) {};
    private:
        SendRequest info;
        napi_ref cb;
        napi_async_work worker;
    };
}


