
#include <cstring>
#include "nipc.h"
namespace NIPC{
    napi_value create_n_error(napi_env env, const char *str) {
        CharToNs(result,str,env);
        CharToNs(str_result,str,env);
        CHECK_NAPI_RESULT(napi_create_error(env, nullptr, str_result, &result));
        return result;
    }

    int get_shared_memory(const char *key_str, size_t size) {
        key_t key = ftok(key_str, 24);
        return shmget(key, size, IPC_CREAT | 0666);
    }

    bool write_to_memory(int id, NChar value) {
        char *shared = (char *) shmat(id, nullptr, 0);
        if (shared == (void *) -1) {
            shmdt(shared);
            shmctl(id, IPC_RMID, nullptr);
            return false;
        }
        memset(shared, '\0', value.getSize());
        strncpy(shared, value.getData(), value.getSize());
        return true;
    }

    napi_value read_from_memory(napi_env env, int id) {
        char *shared = (char *) shmat(id, nullptr, SHM_RDONLY);
        if (!*shared) {
            shmdt(shared);
            shmctl(id, IPC_RMID, nullptr);
            return nullptr;
        }
        CharToNs(result,shared,env);
        shmdt(shared);
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
        int id = get_shared_memory(channel.getData(), 1024);
        if (!write_to_memory(id, value)) {
            napi_throw(env, create_n_error(env, "write to memory error!"));
            return nullptr;
        }
        CharToNs(result,value.getData(),env);
        return result;
    }

    napi_value read(napi_env env, napi_callback_info info) {
        size_t argc = 1;
        napi_value argv[1];
        CHECK_NAPI_RESULT(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
        napi_value channel_n = argv[0];
        NsToChar(channel, channel_n, channel);
        int id = get_shared_memory(channel.getData(), 1024);
        napi_value result = read_from_memory(env, id);
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

