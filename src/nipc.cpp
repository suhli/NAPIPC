#include "nipc.h"
#include "wtf.h"
namespace NIPC{
    NapiValue createNError(napi_env env, const char *str) {
        CharToNs(result,str,env);
        CharToNs(str_result,str,env);
        CHECK_NAPI_RESULT(napi_create_error(env, nullptr, str_result, &result));
        return result;
    }

    int getSharedMemory(const char *key_str, size_t size,int flg) {
        key_t key = ftok(key_str, 65);
        return shmget(key, size, flg);
    }

    IPCPacket pack(std::string value){
        return IPCPacket(value.size(),value);
    }

    void writeSharedMemory(int id, IPCPacket packet) {
        auto *shared = static_cast<IPCPacket *>(shmat(id, nullptr, 0));
        if (shared == (void *) -1) {
            shmdt(shared);
            shmctl(id, IPC_RMID, nullptr);
            return;
        }
        shared->setSize(packet.getSize());
        shared->setContent(std::string(packet.getContent()));
        shmdt(shared);
    }

    NapiValue readSharedMemory(napi_env env, char *channel) {
        int id = getSharedMemory(channel, 0,0);
        void *buf = shmat(id, nullptr, SHM_RDONLY);
        if(buf == (void *) -1){
            shmdt(buf);
            shmctl(id, IPC_RMID, nullptr);
            return nullptr;
        }
        auto *packet = (IPCPacket*) buf;
        CharToNs(result,packet->getContent().c_str(),env);
        shmdt(buf);
        shmctl(id, IPC_RMID, nullptr);
        return result;
    }

    void sendRequest(void *data){
        auto *request = static_cast<SendRequest *>(data);
        IPCPacket packet = pack(request->getValue());
        size_t packet_size = sizeof(packet);
        int id = getSharedMemory(request->getChannel().c_str(), packet_size,IPC_CREAT | 0666);
        writeSharedMemory(id, packet);
    }

    void sendAsyncInWork(napi_env env,void* data){
        auto *worker = (AsyncWorker<SendRequest>*) data;
        sendRequest(worker->getData());
    }

    NapiValue send(napi_env env, napi_callback_info info) {
        size_t argc = 2;
        NapiValue argv[argc];
        CHECK_NAPI_RESULT(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        NapiValue channel_n = argv[0], value_n = argv[1];
        NsToChar(channel, channel_n, channel);
        NsToChar(value, value_n, value);
        SendRequest sendInfo(std::string(channel.getData()),std::string(value.getData()));
        sendRequest(&sendInfo);
        CharToNs(result,value.getData(),env);
        return result;
    }

    void completeAsync(napi_env env,
                        napi_status status,
                        void *data){
        auto *worker = static_cast<AsyncWorker<SendRequest> *>(data);
        CHECK_NAPI_RESULT(status);
        NapiValue global;
        napi_get_global(env,&global);
        NapiValue callback;
        napi_get_reference_value(env,worker->getCb(),&callback);
        CharToNs(cb_status,"success",env);
        auto *request = static_cast< SendRequest *>(worker->getData());
        CharToNs(result,request->getValue().c_str(),env);
        NapiValue _args[] = {cb_status,result};
        NapiValue call_result;
        CHECK_NAPI_RESULT(napi_call_function(env,global,callback,2,_args,&call_result));
        CHECK_NAPI_RESULT(napi_delete_reference(env,worker->getCb()));
        delete worker;
    }


    NapiValue sendAsync(napi_env env, napi_callback_info info){
        size_t argc = 3;
        NapiValue argv[argc];
        CHECK_NAPI_RESULT(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        NapiValue channel_n = argv[0], value_n = argv[1],cb = argv[2];
        NsToChar(channel, channel_n, channel);
        NsToChar(value, value_n, value);
        auto *req = new SendRequest(std::string(channel.getData()),std::string(value.getData()));
        CharToNs(status,"pending",env);
        CharToNs(type,"NAPIPC",env);
        auto *worker = new AsyncWorker<SendRequest>(req);
        NapiRef cb_ref;
        CHECK_NAPI_RESULT(napi_create_reference(env,cb,1,&cb_ref));
        NapiAsyncWork async_work;
        CHECK_NAPI_RESULT(napi_create_async_work(
                env,
                status,
                type,
                sendAsyncInWork,
                completeAsync,
                worker,
                &async_work
        ));
        worker->setCb(cb_ref);
        worker->setWorker(async_work);
        NapiValue n_null;
        CHECK_NAPI_RESULT(napi_get_null(env,&n_null));
        CHECK_NAPI_RESULT(napi_queue_async_work(env,worker->getWorker()));
        return n_null;
    }

    NapiValue read(napi_env env, napi_callback_info info) {
        size_t argc = 1;
        NapiValue argv[1];
        CHECK_NAPI_RESULT(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        NapiValue channel_n = argv[0];
        NsToChar(channel, channel_n, channel);
        NapiValue result = readSharedMemory(env, channel.getData());
        return result;
    }

    NapiValue Init(napi_env env, NapiValue exports) {
        napi_property_descriptor sendProp =
                {"send", nullptr, send, nullptr, nullptr, nullptr, napi_static, nullptr};
        napi_property_descriptor readProp =
                {"read", nullptr, read, nullptr, nullptr, nullptr, napi_static, nullptr};
        napi_property_descriptor asyncProp =
                {"send_async", nullptr, sendAsync, nullptr, nullptr, nullptr, napi_static, nullptr};
        napi_property_descriptor properties[] = {sendProp, readProp,asyncProp};
        napi_define_properties(env, exports, 3, properties);
        return exports;
    }

    NAPI_MODULE(ipc, Init);
}

