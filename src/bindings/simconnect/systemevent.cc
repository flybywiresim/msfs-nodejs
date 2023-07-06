#include <array>
#include <string>

#include "connection.h"
#include "helper.h"
#include "instancedata.h"
#include "systemevent.h"

using namespace msfs::simconnect;

static const std::array<std::string, 24> _systemEventNames = {
    "1sec",
    "4sec",
    "6Hz",
    "AircraftLoaded",
    "Crashed",
    "CrashReset",
    "FlightLoaded",
    "FlightSaved",
    "FlightPlanActivated",
    "FlightPlanDeactivated",
    "Frame",
    "ObjectAdded",
    "ObjectRemoved",
    "Pause",
    "Pause_EX1",
    "Paused",
    "PauseFrame",
    "PositionChanged",
    "Sim",
    "SimStart",
    "SimStop",
    "Sound",
    "Unpaused",
    "View"
};

SystemEvent::SystemEvent(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<SystemEvent>(info),
        _connection(nullptr),
        _event(),
        _id(0),
        _lastError() {
    Napi::Env env = info.Env();

    if (info.Length() != 3) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'connection' must be an object of type Connection").ThrowAsJavaScriptException();
        return;
    }
    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'systemEventId' must be a number").ThrowAsJavaScriptException();
        return;
    }
    if (!info[2].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'eventType' must be a SystemEventType").ThrowAsJavaScriptException();
        return;
    }

    this->_connection = Napi::ObjectWrap<Connection>::Unwrap(info[0].As<Napi::Object>());
    this->_id = static_cast<SIMCONNECT_CLIENT_DATA_ID>(info[1].As<Napi::Number>().Uint32Value());

    if (info[2].As<Napi::Number>().Uint32Value() >= _systemEventNames.size()) {
        Napi::TypeError::New(env, "Invalid argument. 'eventType' exceedes SystemEventType").ThrowAsJavaScriptException();
        return;
    }
    this->_event = _systemEventNames[info[2].As<Napi::Number>().Uint32Value()];

    if (this->_connection->systemEventIdExists(this->_id)) {
        Napi::TypeError::New(env, "The system event ID already exists").ThrowAsJavaScriptException();
        return;
    }

    this->_connection->addSystemEventId(this->_id);
}

std::uint32_t SystemEvent::id() const {
    return this->_id;
}

const std::string& SystemEvent::eventName() const {
    return this->_event;
}

bool SystemEvent::subscribe() {
    HRESULT result = SimConnect_SubscribeToSystemEvent(this->_connection->simConnect(), this->_id, this->_event.c_str());
    return SUCCEEDED(result);
}

bool SystemEvent::unsubscribe() {
    HRESULT result = SimConnect_UnsubscribeFromSystemEvent(this->_connection->simConnect(), this->_id);
    return SUCCEEDED(result);
}

Napi::Value SystemEvent::lastError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, this->_lastError);
}

Napi::Object SystemEvent::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "SystemEventBindings", {
        InstanceMethod<&SystemEvent::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.GetInstanceData<InstanceData>()->clientDataAreaConstructor = constructor;

    exports.Set("SystemEventBindings", func);
    return exports;
}
