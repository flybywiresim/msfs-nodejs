#include "clientdataarea.h"
#include "helper.h"

using namespace msfs::simconnect;

Napi::FunctionReference ClientDataArea::constructor = Napi::FunctionReference();

ClientDataArea::ClientDataArea(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<ClientDataArea>(info),
        _connection(nullptr),
        _lastError() { }

Napi::Value ClientDataArea::mapClientDataNameToId(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'dataId' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsString()) {
        Napi::TypeError::New(env, "Invalid argument type. 'dataName' must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String clientDataName = info[1].As<Napi::String>();
    SIMCONNECT_CLIENT_DATA_ID clientDataId = info[0].As<Napi::Number>().Uint32Value();

    std::string clientDataNameStr = clientDataName.Utf8Value();
    HRESULT result = SimConnect_MapClientDataNameToID(this->_connection->_simConnect, clientDataNameStr.c_str(), clientDataId);
    if (result != S_OK) {
        this->_lastError = "Unable to map the client data ID: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return Napi::Boolean::New(env, false);
    }

    return Napi::Boolean::New(env, true);
}

Napi::Value ClientDataArea::createClientDataArea(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 3) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'clientDataId' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'size' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[2].IsBoolean()) {
        Napi::TypeError::New(env, "Invalid argument type. 'readOnly' must be a boolean").ThrowAsJavaScriptException();
        return env.Null();
    }

    SIMCONNECT_CLIENT_DATA_ID clientDataId = info[0].As<Napi::Number>().Uint32Value();
    DWORD size = info[1].As<Napi::Number>().Uint32Value();
    bool readOnly = info[2].As<Napi::Boolean>().Value();
    SIMCONNECT_CREATE_CLIENT_DATA_FLAG readOnlyFlag = readOnly ?
        SIMCONNECT_CREATE_CLIENT_DATA_FLAG_READ_ONLY :
        SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT;

    HRESULT result = SimConnect_CreateClientData(this->_connection->_simConnect, clientDataId, size, readOnlyFlag);
    if (result != S_OK) {
        this->_lastError = "Unable to create the client data: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return Napi::Boolean::New(env, false);
    }

    return Napi::Boolean::New(env, true);
}

template <typename T>
bool ClientDataArea::setClientDataNumber(SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                  SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId,
                                  const Napi::Value& value) {
    auto number = static_cast<T>(value.As<Napi::Number>().DoubleValue());

    HRESULT result = SimConnect_SetClientData(this->_connection->_simConnect, clientDataId, definitionId,
                                              0, 0, sizeof(T), &number);
    if (result != S_OK) {
        this->_lastError = "Unable to set client data: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return false;
    }

    return true;
}

bool ClientDataArea::setClientDataField(SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                        const Connection::ClientDataDefinition& definition,
                                        const Napi::Object& object) {
    switch (definition.sizeOrType) {
    case SIMCONNECT_CLIENTDATATYPE_INT8:
        return this->setClientDataNumber<std::int8_t>(clientDataId, definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_INT16:
        return this->setClientDataNumber<std::int16_t>(clientDataId, definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_INT32:
        return this->setClientDataNumber<std::int32_t>(clientDataId, definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_INT64:
        return this->setClientDataNumber<std::int64_t>(clientDataId, definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_FLOAT32:
        return this->setClientDataNumber<float>(clientDataId, definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_FLOAT64:
        return this->setClientDataNumber<double>(clientDataId, definition.definitionId, object.Get(definition.memberName));
    default:
    {
        const auto value = object.Get(definition.memberName);
        if (!value.IsArrayBuffer()) {
            this->_lastError = "The member needs to be an ArrayBuffer";
            return false;
        }

        Napi::ArrayBuffer buffer = value.As<Napi::ArrayBuffer>();
        HRESULT result = SimConnect_SetClientData(this->_connection->_simConnect, clientDataId, definition.definitionId,
                                                  0, 0, buffer.ByteLength(), buffer.Data());
        if (result != S_OK) {
            this->_lastError = "Unable to set client data: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
            return false;
        }

        return true;
    }
    }
}

Napi::Value ClientDataArea::setClientData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'clientDataId' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'data' must be an object").ThrowAsJavaScriptException();
        return env.Null();
    }

    SIMCONNECT_CLIENT_DATA_ID clientDataId = info[0].As<Napi::Number>().Uint32Value();
    const auto data = info[1].As<Napi::Object>();
    const auto props = data.GetPropertyNames();

    for (std::uint32_t i = 0; i < props.Length(); i++) {
        bool found = false;

        for (const auto& dataDefinition : std::as_const(this->_connection->_clientDataDefinitions)) {
            if (props.Get(i).As<Napi::String>().Utf8Value() == dataDefinition.memberName) {
                if (false == this->setClientDataField(clientDataId, dataDefinition, data)) {
                    return Napi::Boolean::New(env, false);
                }

                found = true;
            }
        }

        if (false == found) {
            this->_lastError = "Unable to find " + props.Get(i).As<Napi::String>().Utf8Value() + " in the data definitions";
            return Napi::Boolean::New(env, false);
        }
    }

    return Napi::Boolean::New(env, true);
}

Napi::Object ClientDataArea::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ClientDataArea", {
        InstanceMethod<&ClientDataArea::mapClientDataNameToId>("mapClientDataNameToId", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&ClientDataArea::createClientDataArea>("createClientDataArea", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&ClientDataArea::setClientData>("setClientData", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    ClientDataArea::constructor = Napi::Persistent(func);
    ClientDataArea::constructor.SuppressDestruct();

    exports.Set("ClientDataArea", func);
    return exports;
}
