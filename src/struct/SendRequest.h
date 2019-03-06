namespace NIPC {
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
}