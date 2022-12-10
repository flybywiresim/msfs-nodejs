#include <napi.h>

#include "simconnect/connection.h"

using namespace msfs;

void test(const Napi::CallbackInfo& info) { }

Napi::Object initialize(Napi::Env env, Napi::Object exports) {
    exports = simconnect::Connection::initialize(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, initialize)
