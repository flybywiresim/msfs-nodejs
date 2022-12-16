#include "dispatcher.h"
#include "helper.h"
#include "instancedata.h"

using namespace msfs::simconnect;

Dispatcher::Dispatcher(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Dispatcher>(info),
        _connection(nullptr) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'connection' must be an object of type Connection").ThrowAsJavaScriptException();
        return;
    }

    this->_connection = Napi::ObjectWrap<Connection>::Unwrap(info[0].As<Napi::Object>());
}

Napi::Object Dispatcher::convertReceiveOpen(Napi::Env env, SIMCONNECT_RECV_OPEN* open) {
    Napi::Object object = Napi::Object::New(env);

    // set application data
    auto application = Napi::Object::New(env);
    application.Set(Napi::String::New(env, "name"), Napi::String::New(env, open->szApplicationName));
    auto version = Napi::Object::New(env);
    version.Set(Napi::String::New(env, "major"), Napi::Number::New(env, open->dwApplicationVersionMajor));
    version.Set(Napi::String::New(env, "minor"), Napi::Number::New(env, open->dwApplicationVersionMinor));
    application.Set(Napi::String::New(env, "version"), version);
    version = Napi::Object::New(env);
    version.Set(Napi::String::New(env, "major"), Napi::Number::New(env, open->dwApplicationBuildMajor));
    version.Set(Napi::String::New(env, "minor"), Napi::Number::New(env, open->dwApplicationBuildMinor));
    application.Set(Napi::String::New(env, "build"), version);
    object.Set(Napi::String::New(env, "application"), application);

    // set simconnect data
    auto simconnect = Napi::Object::New(env);
    version = Napi::Object::New(env);
    version.Set(Napi::String::New(env, "major"), Napi::Number::New(env, open->dwSimConnectVersionMajor));
    version.Set(Napi::String::New(env, "minor"), Napi::Number::New(env, open->dwSimConnectVersionMinor));
    simconnect.Set(Napi::String::New(env, "version"), version);
    version = Napi::Object::New(env);
    version.Set(Napi::String::New(env, "major"), Napi::Number::New(env, open->dwSimConnectBuildMajor));
    version.Set(Napi::String::New(env, "minor"), Napi::Number::New(env, open->dwSimConnectBuildMinor));
    simconnect.Set(Napi::String::New(env, "build"), version);
    object.Set(Napi::String::New(env, "simconnect"), simconnect);

    return object;
}

Napi::Value Dispatcher::nextDispatch(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->_simConnect == 0) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    SIMCONNECT_RECV* receiveData;
    DWORD size;

    HRESULT result = SimConnect_GetNextDispatch(this->_connection->_simConnect, &receiveData, &size);
    if (result != S_OK) {
        return env.Null();
    }

    Napi::Object object = Napi::Object::New(env);
    bool validObject = true;

    switch (receiveData->dwID) {
    case SIMCONNECT_RECV_ID_OPEN:
        object.Set(Napi::String::New(env, "data"), Dispatcher::convertReceiveOpen(env, static_cast<SIMCONNECT_RECV_OPEN*>(receiveData)));
        object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "open"));
        break;
    case SIMCONNECT_RECV_ID_QUIT:
        object.Set(Napi::String::New(env, "data"), Napi::Object::New(env));
        object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "quit"));
        break;
    default:
        object.Set(Napi::String::New(env, "data"), Napi::Object::New(env));
        object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "unknown"));
        break;
    }

    return object;
}

Napi::Value Dispatcher::lastError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, this->_lastError);
}

Napi::Object Dispatcher::initialize(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "DispatcherBindings", {
        InstanceMethod<&Dispatcher::nextDispatch>("nextDispatch", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Dispatcher::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.GetInstanceData<InstanceData>()->dispatcherConstructor = constructor;

    exports.Set("DispatcherBindings", func);
    return exports;
}
