#include "AsyncWorker.h"

namespace NIPC {
    template<class T>
    void AsyncWorker<T>::onExecute(NapiEnv env, void *data) {
        auto *worker = static_cast<AsyncWorker<T> *>(data);
        worker->execute(env,worker->getData());
    }

    template<class T>
    void AsyncWorker<T>::onComplete(NapiEnv env,NapiStatus status, void *data) {
        auto *worker = static_cast<AsyncWorker<T> *>(data);
        NapiValue callback;
        CHECK_NAPI_RESULT(napi_get_reference_value(env, worker->cb, &callback));
        worker->complete(env, status,worker->data,callback);
        CHECK_NAPI_RESULT(napi_delete_reference(env, worker->cb));
        delete worker;
    }

    template<class T>
    NapiValue AsyncWorker<T>::boot(NapiEnv env,AsyncWorker<T> *worker) {
        NapiAsyncWork work;
        CharToNs(status, "pending", env);
        CharToNs(type, "NAPIPC", env);
        CHECK_NAPI_RESULT(napi_create_async_work(
                env,
                status,
                type,
                worker->onExecute,
                worker->onComplete,
                worker,
                &work
        ));
        worker->work = work;
        NapiValue n_null;
        CHECK_NAPI_RESULT(napi_get_null(env, &n_null));
        CHECK_NAPI_RESULT(napi_queue_async_work(env, worker->work));
        return n_null;
    }
}
