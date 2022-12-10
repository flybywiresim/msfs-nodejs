#include <napi.h>

#include "wrapper.h"

using namespace msfs::simconnect;

void test(const Napi::CallbackInfo& info) { }

Napi::Object initialize(Napi::Env env, Napi::Object exports) {
    exports = Wrapper::initialize(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, initialize)
