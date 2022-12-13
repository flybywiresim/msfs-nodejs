#include "helper.h"
#include "wrapper.h"

#include <SimConnect.h>

using namespace msfs::simconnect;

Wrapper::Wrapper(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Wrapper>(info) { }

Wrapper::~Wrapper() { }

Napi::Value Wrapper::createNewItem(const Napi::CallbackInfo& info) {
    Napi::FunctionReference* constructor = info.Env().GetInstanceData<Napi::FunctionReference>();
    return constructor->New({});
}

Napi::Object Wrapper::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Wrapper", {
        StaticMethod<&Wrapper::createNewItem>("createNewItem", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);
    exports.Set("Wrapper", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}
