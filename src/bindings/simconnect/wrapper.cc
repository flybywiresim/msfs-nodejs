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

void Wrapper::close() {
    if (this->_simConnect != 0) {
        SimConnect_Close(this->_simConnect);
        this->_simConnect = 0;
    }
}

Napi::Value Wrapper::open(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (info[0].IsString()) {
        Napi::TypeError::New(env, "Invalid argument type. 'name' must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (this->_simConnect == 0) {
        Napi::String name = info[0].As<Napi::String>();
        std::string nameStr = name.Utf8Value();
        HRESULT result = SimConnect_Open(&this->_simConnect, nameStr.c_str(), nullptr, 0, 0, 0);
        if (result != S_OK) {
            this->_lastError = "Unable to open a connection: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
            return Napi::Boolean::New(env, false);
        }
    }

    return Napi::Boolean::New(env, true);
}

void Wrapper::close(const Napi::CallbackInfo& info) {
    std::ignore = info;
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
        StaticMethod<&Wrapper::createNewItem>("createNewItem", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);
    exports.Set("SimConnect", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}
