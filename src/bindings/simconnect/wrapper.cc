#include "helper.h"
#include "wrapper.h"

#include <SimConnect.h>

using namespace msfs::simconnect;

Wrapper::Wrapper(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Wrapper>(info),
        _simConnect(0),
        _lastError() { }

Wrapper::~Wrapper() {
    this->close();
}

Napi::Value Wrapper::lastError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->_lastError);
}

Napi::Value Wrapper::createNewItem(const Napi::CallbackInfo& info) {
    Napi::FunctionReference* constructor = info.Env().GetInstanceData<Napi::FunctionReference>();
    return constructor->New({});
}

Napi::Object Wrapper::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "SimConnect", {
        InstanceMethod<&Wrapper::open>("open", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        StaticMethod<&Wrapper::createNewItem>("createNewItem", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);
    exports.Set("SimConnect", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}
