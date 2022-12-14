#include "connection.h"
#include "helper.h"
#include "instancedata.h"

#include <SimConnect.h>

using namespace msfs::simconnect;

Connection::Connection(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Connection>(info),
        _simConnect(0),
        _lastError(),
        _clientDataIds({ 0 }),
        _clientDataDefinitionIdCounter(),
        _clientDataDefinitions() { }

Connection::~Connection() {
    if (this->_simConnect != 0) {
        this->close();
    }
}

void Connection::close() {
    if (this->_simConnect != 0) {
        SimConnect_Close(this->_simConnect);
        this->_simConnect = 0;
    }

    this->_clientDataDefinitionIdCounter = 1;
    this->_clientDataDefinitions.clear();
    this->_clientDataIds = { 0 };
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
            this->_lastError = "Unable to open a connection: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
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

Napi::Value Connection::isConnected(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Boolean::New(env, this->_simConnect != 0);
}

bool Connection::clientDataIdExists(SIMCONNECT_CLIENT_DATA_ID clientDataId) const {
    for (const auto& id : std::as_const(this->_clientDataIds)) {
        if (id == clientDataId)
            return true;
    }

    return false;
}

Napi::Value Connection::newClientDataArea(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_simConnect == 0) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

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

Napi::Value Connection::addClientDataDefinition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_simConnect == 0) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'dataDefinition' must be an object").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto definitionJS = info[0].As<Napi::Object>();
    if (!definitionJS.Has("offset") || !definitionJS.Get("offset").IsNumber()) {
        Napi::TypeError::New(env, "Property not found or invalid. 'offset' needs to be defined as a number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!definitionJS.Has("sizeOrType") || !definitionJS.Get("sizeOrType").IsNumber()) {
        Napi::TypeError::New(env, "Property not found or invalid. 'sizeOrType' needs to be defined as a number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    /* optional property */
    if (definitionJS.Has("epsilon") && !definitionJS.Get("epsilon").IsNumber()) {
        Napi::TypeError::New(env, "Property is invalid. 'epsilon' needs to be defined as number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!definitionJS.Has("memberName") || !definitionJS.Get("memberName").IsString()) {
        Napi::TypeError::New(env, "Property not found or invalid. 'memberName' needs to be defined as a string")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    struct Connection::ClientDataDefinition definition;
    definition.definitionId = this->_clientDataDefinitionIdCounter++;
    definition.offset = definitionJS.Get("offset").As<Napi::Number>().Int32Value();
    definition.sizeOrType = definitionJS.Get("sizeOrType").As<Napi::Number>().Int32Value();
    definition.epsilon = definitionJS.Has("epsilon") ? definitionJS.Get("epsilon").As<Napi::Number>().FloatValue() : 0.0f;
    definition.memberName = definitionJS.Get("memberName").As<Napi::String>();

    HRESULT result = SimConnect_AddToClientDataDefinition(this->_simConnect, definition.definitionId, definition.offset,
                                                          definition.sizeOrType, definition.epsilon);
    if (result != S_OK) {
        this->_lastError = "Unable to add the client data definition: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return Napi::Boolean::New(env, false);
    }

    this->_clientDataDefinitions.push_back(std::move(definition));
    return Napi::Boolean::New(env, true);
}

Napi::Value Connection::clearClientDataDefinition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_simConnect == 0) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Invalid argument type. 'memberName' must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto memberName = info[0].As<Napi::String>().Utf8Value();

    for (auto it = this->_clientDataDefinitions.begin(); it != this->_clientDataDefinitions.end(); ++it) {
        if (it->memberName == memberName) {
            HRESULT result = SimConnect_ClearClientDataDefinition(this->_simConnect, it->definitionId);
            if (result != S_OK) {
                this->_lastError = "Unable to clear the client data definition: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
                return Napi::Boolean::New(env, false);
            }

            this->_clientDataDefinitions.erase(it);
            return Napi::Boolean::New(env, true);
        }
    }

    this->_lastError = "Did not find the data definition";
    return Napi::Boolean::New(env, false);
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
    Napi::Function func = DefineClass(env, "Connection", {
        InstanceMethod<&Connection::open>("open", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::isConnected>("isConnected", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::newClientDataArea>("newClientDataArea", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::addClientDataDefinition>("addClientDataDefinition", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::clearClientDataDefinition>("clearClientDataDefinition", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Connection::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.GetInstanceData<InstanceData>()->connectionConstructor = constructor;

    exports.Set("Connection", func);
    return exports;
}
