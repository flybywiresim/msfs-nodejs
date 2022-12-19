#include "connection.h"
#include "helper.h"
#include "instancedata.h"

#include <SimConnect.h>

using namespace msfs::simconnect;

Connection::Connection(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Connection>(info),
        _simConnect(0),
        _isConnected(false),
        _lastError(),
        _clientDataIds() { }

Connection::~Connection() {
    if (this->_simConnect != 0) {
        this->close();
    }
}

void Connection::close() {
    if (this->_simConnect != 0) {
        SimConnect_Close(this->_simConnect);
        this->_isConnected = false;
        this->_simConnect = 0;
    }
}

HANDLE Connection::simConnect() const {
    return this->_simConnect;
}

void Connection::addClientDataId(SIMCONNECT_CLIENT_DATA_ID clientDataId) {
    this->_clientDataIds.push_back(clientDataId);
}

void Connection::connectionEstablished(bool established) {
    this->_isConnected = established;
}

Napi::Value Connection::open(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Invalid argument type. 'name' must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (this->_simConnect == 0) {
        Napi::String name = info[0].As<Napi::String>();
        std::string nameStr = name.Utf8Value();
        HRESULT result = SimConnect_Open(&this->_simConnect, nameStr.c_str(), nullptr, 0, 0, 0);
        if (result != S_OK) {
            this->_lastError = "Unable to open a connection";
            return Napi::Boolean::New(env, false);
        }
    }

    return Napi::Boolean::New(env, true);
}

void Connection::close(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return;
    }

    this->close();
}

bool Connection::isConnected() const {
    return this->_simConnect != 0 && this->_isConnected == true;
}

Napi::Value Connection::isConnected(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Boolean::New(env, this->isConnected());
}

bool Connection::clientDataIdExists(SIMCONNECT_CLIENT_DATA_ID clientDataId) const {
    for (const auto& id : std::as_const(this->_clientDataIds)) {
        if (id == clientDataId)
            return true;
    }

    return false;
}

Napi::Value Connection::lastError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, this->_lastError);
}

Napi::Object Connection::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ConnectionBindings", {
        InstanceMethod<&Connection::open>("open", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::isConnected>("isConnected", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.GetInstanceData<InstanceData>()->connectionConstructor = constructor;

    exports.Set("ConnectionBindings", func);
    return exports;
}
