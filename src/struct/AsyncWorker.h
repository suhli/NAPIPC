#include <string>
#include <node_api.h>
#include "../wtf.h"
typedef  void (*AsyncWorkExecuteCallback)(NapiEnv,void*);
template<class T>
using AsyncWorkCompleteCallback = void (*)(NapiEnv,NapiStatus,T*,NapiValue);
namespace NIPC {

    template <class T>
    class AsyncWorker {
    public:
        AsyncWorker(T *data,AsyncWorkExecuteCallback execute,AsyncWorkCompleteCallback<T> complete,NapiRef cb) : data(data),execute(execute),complete(complete),cb(cb) {};
        ~AsyncWorker(){
            delete data;
        }
        static NapiValue boot(NapiEnv env,AsyncWorker<T> *worker);
//        AsyncWorkExecuteCallback getExecute(){
//            return execute;
//        };
//        AsyncWorkCompleteCallback<T> getComplete(){
//            return complete;
//        };
        T *getData(){
            return data;
        };
        static void onExecute(NapiEnv env,void *data);
        static void onComplete(NapiEnv env,NapiStatus status,void *data);
    private:
        T *data;
        AsyncWorkExecuteCallback execute;
        AsyncWorkCompleteCallback<T> complete;
        NapiRef cb;
        NapiAsyncWork work;
    };
}


