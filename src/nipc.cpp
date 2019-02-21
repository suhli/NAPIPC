
#include <cstring>
#include "nipc.h"
namespace NIPC{
    napi_value create_n_error(napi_env env, const char *str) {
        CharToNs(result,str,env);
        CharToNs(str_result,str,env);
        CHECK_NAPI_RESULT(napi_create_error(env, nullptr, str_result, &result));
        return result;
    }

    int get_shared_memory(const char *key_str, size_t size,int flg) {
        key_t key = ftok(key_str, 24);
        return shmget(key, size, flg);
    }

    IPCPacket *pack(NChar value){
        auto *packet = static_cast<IPCPacket *>(malloc(sizeof(IPCPacket) + value.getSize()));
        packet->content_len = value.getSize();
        strncpy(packet->content,value.getData(),value.getSize());
        return packet;
    }

    napi_value write_to_memory(int id, IPCPacket *packet,size_t size,napi_env env) {
        auto *shared = static_cast<IPCPacket *>(shmat(id, nullptr, 0));
        if (shared == (void *) -1) {
            shmdt(shared);
            shmctl(id, IPC_RMID, nullptr);
            return nullptr;
        }
        shared->content_len = packet->content_len;
        strncpy(shared->content,packet->content,packet->content_len);
        CharToNs(result,shared->content,env);
        shmdt(shared);
        return result;
    }

    napi_value read_from_memory(napi_env env, char *channel) {
        int id = get_shared_memory(channel, 0,0);
        void *buf = shmat(id, nullptr, SHM_RDONLY);
        if(buf == (void *) -1){
            shmdt(buf);
            shmctl(id, IPC_RMID, nullptr);
            return nullptr;
        }
        auto *packet = reinterpret_cast<IPCPacket*>(buf);
        CharToNs(result,packet->content,env);
        shmdt(buf);
        shmctl(id, IPC_RMID, nullptr);
        return result;
    }

    napi_value send(napi_env env, napi_callback_info info) {
        size_t argc = 2;
        napi_value argv[2];
        CHECK_NAPI_RESULT(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        napi_value channel_n = argv[0], value_n = argv[1];
        NsToChar(channel, channel_n, channel);
        NsToChar(value, value_n, value);
        IPCPacket *packet = pack(value);
        size_t packet_size = sizeof(*packet);
        int id = get_shared_memory(channel.getData(), packet_size,IPC_CREAT | 0666);
        return write_to_memory(id, packet,packet_size,env);
    }

    napi_value read(napi_env env, napi_callback_info info) {
        size_t argc = 1;
        napi_value argv[1];
        CHECK_NAPI_RESULT(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        napi_value channel_n = argv[0];
        NsToChar(channel, channel_n, channel);
        napi_value result = read_from_memory(env, channel.getData());
        return result;
    }

    napi_value Init(napi_env env, napi_value exports) {
        napi_property_descriptor sendProp =
                {"send", nullptr, send, nullptr, nullptr, nullptr, napi_default, nullptr};
        napi_property_descriptor readProp =
                {"read", nullptr, read, nullptr, nullptr, nullptr, napi_default, nullptr};
        napi_property_descriptor properties[] = {sendProp, readProp};
        napi_define_properties(env, exports, 2, properties);
        return exports;
    }

    NAPI_MODULE(ipc, Init);
}

