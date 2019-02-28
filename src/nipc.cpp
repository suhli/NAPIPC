
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
        key_t key = ftok(key_str, 65);
        return shmget(key, size, flg);
    }

    IPCPacket *pack(NChar value){
        auto *packet = static_cast<IPCPacket *>(malloc(sizeof(IPCPacket) + value.getSize()));
        packet->content_len = value.getSize();
        strncpy(packet->content,value.getData(),value.getSize());
        return packet;
    }

    char* write_to_memory(int id, IPCPacket *packet,napi_env env) {
        auto *shared = static_cast<IPCPacket *>(shmat(id, nullptr, 0));
        if (shared == (void *) -1) {
            shmdt(shared);
            shmctl(id, IPC_RMID, nullptr);
            return nullptr;
        }
        shared->content_len = packet->content_len;
        strncpy(shared->content,packet->content,packet->content_len);
        shmdt(shared);
        free(packet);
        return shared->content;
    }

    napi_value read_from_memory(napi_env env, char *channel) {

        int id = get_shared_memory(channel, 0,0);
        void *buf = shmat(id, nullptr, SHM_RDONLY);
        if(buf == (void *) -1){
            shmdt(buf);
            shmctl(id, IPC_RMID, nullptr);
            return nullptr;
        }
        auto *packet = (IPCPacket*) buf;
        CharToNs(result,packet->content,env);
        shmdt(buf);
        shmctl(id, IPC_RMID, nullptr);
        return result;
    }

    char* send_by_info(napi_env env,SendInfo *info){
        IPCPacket *packet = pack(info->value);
        size_t packet_size = sizeof(*packet);
        int id = get_shared_memory(info->channel.getData(), packet_size,IPC_CREAT | 0666);
        write_to_memory(id, packet,env);
        return info->value.getData();
    }

    void send_async_in_work(napi_env env,void* data){
        auto *worker = (AsyncWorker*) data;
        send_by_info(env,worker->info);
    }

    napi_value send(napi_env env, napi_callback_info info) {
        size_t argc = 2;
        napi_value argv[argc];
        CHECK_NAPI_RESULT(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        napi_value channel_n = argv[0], value_n = argv[1];
        NsToChar(channel, channel_n, channel);
        NsToChar(value, value_n, value);
        auto send_info = (SendInfo *) malloc(sizeof(SendInfo) + sizeof(value) + sizeof(channel));
        send_info->value = value;
        send_info->channel = channel;
        char *send_result = send_by_info(env,send_info);
        CharToNs(result,send_result,env);
        free(send_info);
        return result;
    }

    void complete_async(napi_env env,
                        napi_status status,
                        void *data){
        auto *worker = (AsyncWorker*) data;
        CHECK_NAPI_RESULT(status);
        napi_value global;
        napi_get_global(env,&global);
        napi_value callback;
        napi_get_reference_value(env,worker->cb,&callback);
        CharToNs(cb_status,"success",env);
        CharToNs(result,worker->info->value.getData(),env);
        napi_value _args[] = {cb_status,result};
        napi_value call_result;
        CHECK_NAPI_RESULT(napi_call_function(env,global,callback,2,_args,&call_result));
        CHECK_NAPI_RESULT(napi_delete_reference(env,worker->cb));
        free(worker->info);
        free(worker);
    }


    napi_value send_async(napi_env env, napi_callback_info info){
        size_t argc = 3;
        napi_value argv[argc];
        CHECK_NAPI_RESULT(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        napi_value channel_n = argv[0], value_n = argv[1],cb = argv[2];
        NsToChar(channel, channel_n, channel);
        NsToChar(value, value_n, value);
        auto send_info = (SendInfo *) malloc(sizeof(SendInfo) + sizeof(value) + sizeof(channel));
        send_info->value = value;
        send_info->channel = channel;
        CharToNs(status,"pending",env);
        CharToNs(type,"NAPIPC",env);
        auto worker = (AsyncWorker*)malloc(sizeof(AsyncWorker));
        worker->info = send_info;
        CHECK_NAPI_RESULT(napi_create_reference(env,cb,1,&worker->cb));
        CHECK_NAPI_RESULT(napi_create_async_work(
                env,
                status,
                type,
                send_async_in_work,
                complete_async,
                worker,
                &worker->worker
        ));
        napi_value n_null;
        CHECK_NAPI_RESULT(napi_get_null(env,&n_null));
        CHECK_NAPI_RESULT(napi_queue_async_work(env,worker->worker));
        return n_null;
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
                {"send", nullptr, send, nullptr, nullptr, nullptr, napi_static, nullptr};
        napi_property_descriptor readProp =
                {"read", nullptr, read, nullptr, nullptr, nullptr, napi_static, nullptr};
        napi_property_descriptor asyncProp =
                {"send_async", nullptr, send_async, nullptr, nullptr, nullptr, napi_static, nullptr};
        napi_property_descriptor properties[] = {sendProp, readProp,asyncProp};
        napi_define_properties(env, exports, 3, properties);
        return exports;
    }

    NAPI_MODULE(ipc, Init);
}

