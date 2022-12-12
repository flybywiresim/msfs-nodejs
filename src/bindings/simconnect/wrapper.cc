#include "helper.h"
#include "wrapper.h"

#include <SimConnect.h>

using namespace msfs::simconnect;

Wrapper::Wrapper(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Wrapper>(info),
        _simConnect(0),
        _clientDataIds(),
        _clientDataDefinitionIdCounter(1),
        _clientDataDefinitions(),
        _lastError() { }

Wrapper::~Wrapper() {
    this->close();
}

bool Wrapper::clientDataIdExists(SIMCONNECT_CLIENT_DATA_ID clientDataId) const {
    for (const auto& id : std::as_const(this->_clientDataIds)) {
        if (id == clientDataId)
            return true;
    }

    return false;
}

Napi::Value Wrapper::newClientDataArea(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'clientDataId' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const SIMCONNECT_CLIENT_DATA_ID clientDataId = info[0].As<Napi::Number>().Uint32Value();

    if (this->clientDataIdExists(clientDataId) == true) {
        this->_lastError = "Client data ID is already in use";
        return Napi::Boolean::New(env, false);
    }

    this->_clientDataIds.push_back(clientDataId);
    return Napi::Boolean::New(env, true);
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
    Napi::Function func = DefineClass(env, "Wrapper", {
        InstanceMethod<&Wrapper::open>("open", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::isConnected>("isConnected", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::newClientDataArea>("newClientDataArea", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::mapClientDataNameToId>("mapClientDataNameToId", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::addClientDataDefinition>("addClientDataDefinition", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::createClientDataArea>("createClientDataArea", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::setClientData>("setClientData", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Wrapper::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        StaticMethod<&Wrapper::createNewItem>("createNewItem", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);
    exports.Set("Wrapper", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}
