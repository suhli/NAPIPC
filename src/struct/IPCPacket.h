//
// Created by shuhang on 2019/3/5.
//
namespace NIPC {
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
}