#include "clientdataarea.h"
#include "connection.h"
#include "helper.h"
#include "instancedata.h"

using namespace msfs::simconnect;

ClientDataArea::ClientDataArea(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<ClientDataArea>(info),
        _connection(nullptr),
        _id(0),
        _clientDataDefinitions(),
        _lastError() {
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'connection' must be an object of type Connection").ThrowAsJavaScriptException();
        return;
    }
    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'clientDataId' must be a number").ThrowAsJavaScriptException();
        return;
    }

    this->_connection = Napi::ObjectWrap<Connection>::Unwrap(info[0].As<Napi::Object>());
    this->_id = static_cast<SIMCONNECT_CLIENT_DATA_ID>(info[1].As<Napi::Number>().Uint32Value());

    if (this->_connection->clientDataIdExists(this->_id)) {
        Napi::TypeError::New(env, "The client data ID already exists").ThrowAsJavaScriptException();
        return;
    }

    this->_connection->addClientDataId(this->_id);
}

const SIMCONNECT_CLIENT_DATA_ID& ClientDataArea::id() const {
    return this->_id;
}

Napi::Value ClientDataArea::mapNameToId(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->isConnected() == false) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Invalid argument type. 'dataName' must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String clientDataName = info[0].As<Napi::String>();

    std::string clientDataNameStr = clientDataName.Utf8Value();
    HRESULT result = SimConnect_MapClientDataNameToID(this->_connection->simConnect(), clientDataNameStr.c_str(), this->_id);
    if (result != S_OK) {
        this->_lastError = "Unable to map the client data ID";
        return Napi::Boolean::New(env, false);
    }

    return Napi::Boolean::New(env, true);
}

Napi::Value ClientDataArea::addDataDefinition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->isConnected() == false) {
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
    if (!definitionJS.Has("definitionId") || !definitionJS.Get("definitionId").IsNumber()) {
        Napi::TypeError::New(env, "Property not found or invalid. 'definitionId' needs to be defined as a number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
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

    struct ClientDataDefinition definition;
    definition.definitionId = definitionJS.Get("definitionId").As<Napi::Number>().Int32Value();
    definition.offset = definitionJS.Get("offset").As<Napi::Number>().Int32Value();
    definition.sizeOrType = definitionJS.Get("sizeOrType").As<Napi::Number>().Int32Value();
    definition.epsilon = definitionJS.Has("epsilon") ? definitionJS.Get("epsilon").As<Napi::Number>().FloatValue() : 0.0f;
    definition.memberName = definitionJS.Get("memberName").As<Napi::String>();

    HRESULT result = SimConnect_AddToClientDataDefinition(this->_connection->simConnect(), definition.definitionId, definition.offset,
                                                          definition.sizeOrType, definition.epsilon);
    if (result != S_OK) {
        this->_lastError = "Unable to add the client data definition";
        return Napi::Boolean::New(env, false);
    }

    this->_clientDataDefinitions.push_back(std::move(definition));
    return Napi::Boolean::New(env, true);
}

Napi::Value ClientDataArea::allocateArea(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->isConnected() == false) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'size' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsBoolean()) {
        Napi::TypeError::New(env, "Invalid argument type. 'readOnly' must be a boolean").ThrowAsJavaScriptException();
        return env.Null();
    }

    DWORD size = info[0].As<Napi::Number>().Uint32Value();
    bool readOnly = info[1].As<Napi::Boolean>().Value();
    SIMCONNECT_CREATE_CLIENT_DATA_FLAG readOnlyFlag = readOnly ?
        SIMCONNECT_CREATE_CLIENT_DATA_FLAG_READ_ONLY :
        SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT;

    HRESULT result = SimConnect_CreateClientData(this->_connection->simConnect(), this->_id, size, readOnlyFlag);
    if (result != S_OK) {
        this->_lastError = "Unable to create the client data";
        return Napi::Boolean::New(env, false);
    }

    return Napi::Boolean::New(env, true);
}

template <typename T>
bool ClientDataArea::setClientDataNumber(SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId,
                                         const Napi::Value& value) {
    auto number = static_cast<T>(value.As<Napi::Number>().DoubleValue());

    HRESULT result = SimConnect_SetClientData(this->_connection->simConnect(), this->_id, definitionId,
                                              SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT, 0, sizeof(T), &number);
    if (result != S_OK) {
        this->_lastError = "Unable to set client data";
        return false;
    }

    return true;
}

bool ClientDataArea::setClientDataField(const ClientDataDefinition& definition,
                                        const Napi::Object& object) {
    switch (definition.sizeOrType) {
    case SIMCONNECT_CLIENTDATATYPE_INT8:
        return this->setClientDataNumber<std::int8_t>(definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_INT16:
        return this->setClientDataNumber<std::int16_t>(definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_INT32:
        return this->setClientDataNumber<std::int32_t>(definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_INT64:
        return this->setClientDataNumber<std::int64_t>(definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_FLOAT32:
        return this->setClientDataNumber<float>(definition.definitionId, object.Get(definition.memberName));
    case SIMCONNECT_CLIENTDATATYPE_FLOAT64:
        return this->setClientDataNumber<double>(definition.definitionId, object.Get(definition.memberName));
    default:
    {
        const auto value = object.Get(definition.memberName);
        if (!value.IsArrayBuffer()) {
            this->_lastError = "The member needs to be an ArrayBuffer";
            return false;
        }

        Napi::ArrayBuffer buffer = value.As<Napi::ArrayBuffer>();
        HRESULT result = SimConnect_SetClientData(this->_connection->simConnect(), this->_id, definition.definitionId,
                                                  SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT, 0, buffer.ByteLength(), buffer.Data());
        if (result != S_OK) {
            this->_lastError = "Unable to set client data";
            return false;
        }

        return true;
    }
    }
}

Napi::Value ClientDataArea::setData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->isConnected() == false) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'data' must be an object").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto data = info[0].As<Napi::Object>();
    const auto props = data.GetPropertyNames();

    for (std::uint32_t i = 0; i < props.Length(); i++) {
        bool found = false;

        for (const auto& dataDefinition : std::as_const(this->_clientDataDefinitions)) {
            if (props.Get(i).As<Napi::String>().Utf8Value() == dataDefinition.memberName) {
                if (false == this->setClientDataField(dataDefinition, data)) {
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

bool ClientDataArea::requestData(SIMCONNECT_DATA_REQUEST_ID& requestId, SIMCONNECT_CLIENT_DATA_PERIOD period,
                                 SIMCONNECT_DATA_REQUEST_FLAG flag) {
    for (auto& member : this->_clientDataDefinitions) {
        member.requestId = requestId++;
    }

    return this->updateRequestData(period, flag);
}

bool ClientDataArea::updateRequestData(SIMCONNECT_CLIENT_DATA_PERIOD period, SIMCONNECT_DATA_REQUEST_FLAG flag) {
    for (auto& member : this->_clientDataDefinitions) {
        HRESULT result = SimConnect_RequestClientData(this->_connection->simConnect(), this->_id, member.requestId,
                                                      member.definitionId, period, flag);
        if (result != S_OK) {
            this->_lastError = "Unable to request client data";
            return false;
        }
    }

    return true;
}

Napi::Value ClientDataArea::lastError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, this->_lastError);
}

Napi::Object ClientDataArea::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ClientDataAreaBindings", {
        InstanceMethod<&ClientDataArea::mapNameToId>("mapNameToId", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&ClientDataArea::allocateArea>("allocateArea", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&ClientDataArea::addDataDefinition>("addDataDefinition", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&ClientDataArea::setData>("setData", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&ClientDataArea::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.GetInstanceData<InstanceData>()->clientDataAreaConstructor = constructor;

    exports.Set("ClientDataAreaBindings", func);
    return exports;
}
