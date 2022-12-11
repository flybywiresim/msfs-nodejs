#include "helper.h"
#include "wrapper.h"

using namespace msfs::simconnect;

Napi::Value Wrapper::mapClientDataNameToId(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Invalid argument type. 'dataName' must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'dataId' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String clientDataName = info[0].As<Napi::String>();
    SIMCONNECT_CLIENT_DATA_ID clientDataId = info[1].As<Napi::Number>().Uint32Value();

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

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'clientDataId' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'dataDefinition' must be an object").ThrowAsJavaScriptException();
        return env.Null();
    }

    const SIMCONNECT_CLIENT_DATA_ID clientDataId = info[0].As<Napi::Number>().Uint32Value();

    const auto definitionJS = info[1].As<Napi::Object>();
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

    struct Wrapper::DataDefinition definition;
    definition.definitionId = definitionJS.Get("definitionId").As<Napi::Number>().Uint32Value();
    definition.offset = definitionJS.Get("offset").As<Napi::Number>().Int32Value();
    definition.sizeOrType = definitionJS.Get("sizeOrType").As<Napi::Number>().Int32Value();
    definition.epsilon = definitionJS.Has("epsilon") ? definitionJS.Get("epsilon").As<Napi::Number>().FloatValue() : 0.0f;
    definition.memberName = definitionJS.Get("memberName").As<Napi::String>();

    for (auto& dataArea : this->_clientDataAreas) {
        if (dataArea.first == clientDataId) {
            /* check if the definition exists */
            for (const auto& dataDefinition : std::as_const(dataArea.second)) {
                if (dataDefinition.definitionId == definition.definitionId) {
                    this->_lastError = "Definition ID already in use.";
                    return Napi::Boolean::New(env, false);
                }
            }

            HRESULT result = SimConnect_AddToClientDataDefinition(this->_simConnect, definition.definitionId, definition.offset,
                                                                  definition.sizeOrType, definition.epsilon);
            if (result != S_OK) {
                this->_lastError = "Unable to add the client data definition: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
                return Napi::Boolean::New(env, false);
            }

            dataArea.second.push_back(std::move(definition));
            return Napi::Boolean::New(env, true);
        }
    }

    this->_lastError = "Client data ID not found. Call 'newClientDataArea' first";
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
