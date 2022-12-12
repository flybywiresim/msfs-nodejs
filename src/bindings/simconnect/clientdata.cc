#include "helper.h"
#include "wrapper.h"

using namespace msfs::simconnect;

Napi::Value Wrapper::mapClientDataNameToId(const Napi::CallbackInfo& info) {
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

    if (this->clientDataIdExists(clientDataId) == false) {
        this->_lastError = "Client data ID not found. Call 'newClientDataArea' first";
        return Napi::Boolean::New(env, false);
    }

    std::string clientDataNameStr = clientDataName.Utf8Value();
    HRESULT result = SimConnect_MapClientDataNameToID(&this->_simConnect, clientDataNameStr.c_str(), clientDataId);
    if (result != S_OK) {
        this->_lastError = "Unable to map the client data ID: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return Napi::Boolean::New(env, false);
    }

    return Napi::Boolean::New(env, true);
}

Napi::Value Wrapper::addClientDataDefinition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

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

    struct Wrapper::ClientDataDefinition definition;
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

Napi::Value Wrapper::clearClientDataDefinition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Invalid argument type. 'memberName' must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto memberName = info[0].As<Napi::String>();

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

Napi::Value Wrapper::createClientDataArea(const Napi::CallbackInfo& info) {
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
    if (!info[1].IsBoolean()) {
        Napi::TypeError::New(env, "Invalid argument type. 'readOnly' must be a boolean").ThrowAsJavaScriptException();
        return env.Null();
    }

    SIMCONNECT_CLIENT_DATA_ID clientDataId = info[0].As<Napi::Number>().Uint32Value();
    DWORD size = info[1].As<Napi::Number>().Uint32Value();
    bool readOnly = info[2].As<Napi::Boolean>().Value();
    SIMCONNECT_CREATE_CLIENT_DATA_FLAG readOnlyFlag = readOnly ?
        SIMCONNECT_CREATE_CLIENT_DATA_FLAG_READ_ONLY :
        SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT;

    if (this->clientDataIdExists(clientDataId) == false) {
        this->_lastError = "Client data ID not found. Call 'newClientDataArea' first";
        return Napi::Boolean::New(env, false);
    }

    HRESULT result = SimConnect_CreateClientData(this->_simConnect, clientDataId, size, readOnlyFlag);
    if (result != S_OK) {
        this->_lastError = "Unable to create the client data: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return Napi::Boolean::New(env, false);
    }

    return Napi::Boolean::New(env, true);
}

template <typename T>
bool Wrapper::setClientDataNumber(SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                  SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId,
                                  const Napi::Value& value) {
    auto number = static_cast<T>(value.As<Napi::Number>().DoubleValue());

    HRESULT result = SimConnect_SetClientData(this->_simConnect, clientDataId, definitionId,
                                              0, 0, sizeof(T), &number);
    if (result != S_OK) {
        this->_lastError = "Unable to set client data: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return false;
    }

    return true;
}

bool Wrapper::setClientDataField(SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                 const Wrapper::ClientDataDefinition& definition,
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
        HRESULT result = SimConnect_SetClientData(this->_simConnect, clientDataId, definition.definitionId,
                                                  0, 0, buffer.ByteLength(), buffer.Data());
        if (result != S_OK) {
            this->_lastError = "Unable to set client data: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
            return false;
        }

        return true;
    }
    }
}

Napi::Value Wrapper::setClientData(const Napi::CallbackInfo& info) {
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

    bool dataSent = false;

    for (const auto& dataDefinition : std::as_const(this->_clientDataDefinitions)) {
        /* found a new member entry*/
        if (data.Has(dataDefinition.memberName)) {
            if (false == this->setClientDataField(clientDataId, dataDefinition, data)) {
                return Napi::Boolean::New(env, false);
            }

            dataSent = true;
        }
    }

    if (!dataSent) {
        this->_lastError = "No member of the data found int the client data definitions";
    }
    return Napi::Boolean::New(env, dataSent);
}
