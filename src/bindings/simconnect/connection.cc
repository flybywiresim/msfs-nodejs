#include "helper.h"
#include "connection.h"

#include <SimConnect.h>

using namespace msfs::simconnect;

Connection::Connection(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Connection>(info),
        _simConnect(0),
        _lastError() { }

Connection::~Connection() {
    this->close();
}

void Connection::close() {
    if (this->_simConnect != 0) {
        SimConnect_Close(this->_simConnect);
        this->_simConnect = 0;
    }
}

Napi::Value Connection::open(const Napi::CallbackInfo& info) {
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

void Connection::close(const Napi::CallbackInfo& info) {
    std::ignore = info;
    this->close();
}

Napi::Value Connection::lastError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->_lastError);
}

Napi::Value Connection::createNewItem(const Napi::CallbackInfo& info) {
    Napi::FunctionReference* constructor = info.Env().GetInstanceData<Napi::FunctionReference>();
    return constructor->New({});
}

Napi::Object Connection::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "SimConnect_Connection", {
        InstanceMethod<&Connection::open>("open", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        StaticMethod<&Connection::createNewItem>("createNewItem", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);
    exports.Set("SimConnect_Connection", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}
