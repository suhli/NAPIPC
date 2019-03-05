//
// Created by shuhang on 2018/10/24.
//

#include <node_api.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdio>
#include <cassert>
#include "struct.h"
#define CHECK_NAPI_RESULT(condition) (assert((condition) == napi_ok))

typedef napi_value NapiValue;
typedef napi_ref NapiRef;
typedef napi_async_work NapiAsyncWork;
typedef size_t sizeT;

#define CharToNs(to, from, env)                                                             \
  napi_value to ## VAL;                                                                     \
  {                                                                                         \
   CHECK_NAPI_RESULT(napi_create_string_utf8(env,from,NAPI_AUTO_LENGTH,&to ## VAL));        \
  }                                                                                         \
  napi_value to(to ## VAL)

#define VoidToNBuf(to, size, data, env)                                                             \
  napi_value to ## VAL;                                                                     \
  {                                                                                         \
   CHECK_NAPI_RESULT(napi_create_buffer(env,size,data,&to ## VAL));        \
  }                                                                                         \
  napi_value to(to ## VAL)

#define NsToChar(to, from, name)                                                            \
  size_t to ## Sz_;                                                                         \
  char* to ## Ch_;                                                                          \
  {                                                                                         \
    napi_valuetype from ## Type_;                                                           \
    CHECK_NAPI_RESULT(napi_typeof(env, from, &(from ## Type_)));                            \
    if (from ## Type_ == napi_null || from ## Type_ == napi_undefined) {                    \
      to ## Sz_ = 0;                                                                        \
      to ## Ch_ = 0;                                                                        \
    } else {                                                                                \
      napi_value from ## Object_ ;                                                          \
      CHECK_NAPI_RESULT(napi_coerce_to_object(env, from, &(from ## Object_)));              \
      bool result = false;                                                                  \
      if (from ## Object_ != nullptr) {                                                     \
        CHECK_NAPI_RESULT(napi_is_buffer(env, from ## Object_, &result));                   \
      }                                                                                     \
      if (result) {                                                                         \
        CHECK_NAPI_RESULT(napi_get_buffer_info(                                             \
          env, from ## Object_, (void**)&(to ## Ch_), &(to ## Sz_)));                       \
      } else {                                                                              \
        napi_value to ## Str_;                                                              \
        CHECK_NAPI_RESULT(napi_coerce_to_string(env, from, &(to ## Str_)));                 \
        size_t sz;                                                                          \
        CHECK_NAPI_RESULT(napi_get_value_string_utf8(env, to ## Str_, nullptr, 0, &sz));    \
        to ## Sz_ = sz;                                                                     \
        to ## Ch_ = new char[to ## Sz_+1];                                                      \
        size_t unused;                                                                      \
        CHECK_NAPI_RESULT(                                                                  \
          napi_get_value_string_utf8(env, to ## Str_, to ## Ch_, to ## Sz_+1, &unused));    \
      }                                                                                     \
    }                                                                                       \
  }                                                                                         \
  NIPC::NChar to(to ## Ch_,to ## Sz_)

