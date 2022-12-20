#include "instancedata.h"
#include "simulatordataarea.h"

using namespace msfs::simconnect;

SimulatorDataArea::SimulatorDataArea(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<SimulatorDataArea>(info),
        _connection(),
        _id(),
        _requestId(),
        _dataDefinitions(),
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
        Napi::TypeError::New(env, "Invalid argument type. 'simulatorDataId' must be a number").ThrowAsJavaScriptException();
        return;
    }

    this->_connection = Napi::ObjectWrap<Connection>::Unwrap(info[0].As<Napi::Object>());
    this->_id = static_cast<SIMCONNECT_DATA_DEFINITION_ID>(info[1].As<Napi::Number>().Uint32Value());

    if (this->_connection->simulatorDataIdExists(this->_id)) {
        Napi::TypeError::New(env, "The simulator data ID already exists").ThrowAsJavaScriptException();
        return;
    }

    this->_connection->addSimulatorDataId(this->_id);
}

const SIMCONNECT_DATA_DEFINITION_ID& SimulatorDataArea::id() const {
    return this->_id;
}

Napi::Value SimulatorDataArea::addDataDefinition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->simConnect() == 0) {
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
    if (!definitionJS.Has("type") || !definitionJS.Get("type").IsNumber()) {
        Napi::TypeError::New(env, "Property not found or invalid. 'type' needs to be defined as a number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!definitionJS.Has("name") || !definitionJS.Get("name").IsString()) {
        Napi::TypeError::New(env, "Property not found or invalid. 'name' needs to be defined as a string")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!definitionJS.Has("unit") || !definitionJS.Get("unit").IsString()) {
        Napi::TypeError::New(env, "Property not found or invalid. 'unit' needs to be defined as a string")
            .ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!definitionJS.Has("memberName") || !definitionJS.Get("memberName").IsString()) {
        Napi::TypeError::New(env, "Property not found or invalid. 'memberName' needs to be defined as a string")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    struct SimulatorDataDefinition definition;
    definition.type = static_cast<SIMCONNECT_DATATYPE>(definitionJS.Get("type").As<Napi::Number>().Uint32Value());
    definition.name = definitionJS.Get("name").As<Napi::String>().Utf8Value();
    definition.unit = definitionJS.Get("unit").As<Napi::String>().Utf8Value();
    definition.memberName = definitionJS.Get("memberName").As<Napi::String>().Utf8Value();

    HRESULT result = SimConnect_AddToDataDefinition(this->_connection->simConnect(), this->_id, definition.name.c_str(),
                                                    definition.unit.c_str());
    if (result != S_OK) {
        this->_lastError = "Unable to add the data definition";
        return Napi::Boolean::New(env, false);
    }

    this->_dataDefinitions.push_back(std::move(definition));
    return Napi::Boolean::New(env, true);
}

bool SimulatorDataArea::requestData(SIMCONNECT_DATA_REQUEST_ID requestId, SIMCONNECT_PERIOD period) {
    this->_requestId = requestId;
    return this->updateRequestData(period);
}

bool SimulatorDataArea::updateRequestData(SIMCONNECT_PERIOD period) {
    HRESULT result = SimConnect_RequestDataOnSimObject(this->_connection->simConnect(), this->_requestId,
                                                       this->_id, SIMCONNECT_OBJECT_ID_USER, period);
    if (result != S_OK) {
        this->_lastError = "Unable to request the simulator data";
        return false;
    }

    return true;
}

Napi::Value SimulatorDataArea::lastError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, this->_lastError);
}

Napi::Object SimulatorDataArea::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "SimulatorDataAreaBindings", {
        InstanceMethod<&SimulatorDataArea::addDataDefinition>("addDataDefinition", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&SimulatorDataArea::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.GetInstanceData<InstanceData>()->simulatorDataAreaConstructor = constructor;

    exports.Set("SimulatorDataAreaBindings", func);
    return exports;
}
