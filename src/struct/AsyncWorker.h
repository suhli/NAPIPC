#include <string>
#include <node_api.h>
namespace NIPC {

    template <class T>
    class AsyncWorker {
    public:
        void * getData() {
            return data;
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

        AsyncWorker(T *data) : data(data) {};
        ~AsyncWorker(){
            delete data;
        }
    private:
        T *data;
        napi_ref cb;
        napi_async_work worker;
    };
}


