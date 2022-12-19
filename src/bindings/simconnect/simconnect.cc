#include <napi.h>

#include "connection.h"
#include "clientdataarea.h"
#include "dispatcher.h"
#include "instancedata.h"

using namespace msfs::simconnect;

void test(const Napi::CallbackInfo& info) { }

Napi::Object initialize(Napi::Env env, Napi::Object exports) {
    env.SetInstanceData(new InstanceData());

    exports = Connection::initialize(env, exports);
    exports = ClientDataArea::initialize(env, exports);
    exports = Dispatcher::initialize(env, exports);

    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, initialize)
