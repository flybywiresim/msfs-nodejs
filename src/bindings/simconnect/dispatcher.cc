#include "dispatcher.h"
#include "helper.h"
#include "instancedata.h"

using namespace msfs::simconnect;

Dispatcher::Dispatcher(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Dispatcher>(info),
        _connection(nullptr),
        _requestedClientAreas(),
        _lastError() {
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

Napi::Object Dispatcher::convertOpenMessage(Napi::Env env, SIMCONNECT_RECV_OPEN* message) {
    Napi::Object object = Napi::Object::New(env);

    // set application data
    auto application = Napi::Object::New(env);
    application.Set(Napi::String::New(env, "name"), Napi::String::New(env, message->szApplicationName));
    auto version = Napi::Object::New(env);
    version.Set(Napi::String::New(env, "major"), Napi::Number::New(env, message->dwApplicationVersionMajor));
    version.Set(Napi::String::New(env, "minor"), Napi::Number::New(env, message->dwApplicationVersionMinor));
    application.Set(Napi::String::New(env, "version"), version);
    version = Napi::Object::New(env);
    version.Set(Napi::String::New(env, "major"), Napi::Number::New(env, message->dwApplicationBuildMajor));
    version.Set(Napi::String::New(env, "minor"), Napi::Number::New(env, message->dwApplicationBuildMinor));
    application.Set(Napi::String::New(env, "build"), version);
    object.Set(Napi::String::New(env, "application"), application);

    // set simconnect data
    auto simconnect = Napi::Object::New(env);
    version = Napi::Object::New(env);
    version.Set(Napi::String::New(env, "major"), Napi::Number::New(env, message->dwSimConnectVersionMajor));
    version.Set(Napi::String::New(env, "minor"), Napi::Number::New(env, message->dwSimConnectVersionMinor));
    simconnect.Set(Napi::String::New(env, "version"), version);
    version = Napi::Object::New(env);
    version.Set(Napi::String::New(env, "major"), Napi::Number::New(env, message->dwSimConnectBuildMajor));
    version.Set(Napi::String::New(env, "minor"), Napi::Number::New(env, message->dwSimConnectBuildMinor));
    simconnect.Set(Napi::String::New(env, "build"), version);
    object.Set(Napi::String::New(env, "simconnect"), simconnect);

    return object;
}

Napi::Object Dispatcher::convertUnknownMessage(Napi::Env env, SIMCONNECT_RECV* message) {
    Napi::Object object = Napi::Object::New(env);

    object.Set(Napi::String::New(env, "id"), Napi::Number::New(env, message->dwID));
    object.Set(Napi::String::New(env, "size"), Napi::Number::New(env, message->dwSize));
    object.Set(Napi::String::New(env, "version"), Napi::Number::New(env, message->dwVersion));

    return object;
}

Napi::Object Dispatcher::convertExceptionMessage(Napi::Env env, SIMCONNECT_RECV_EXCEPTION* message) {
    Napi::Object object = Napi::Object::New(env);

    object.Set(Napi::String::New(env, "exception"), Napi::Number::New(env, message->dwException));
    object.Set(Napi::String::New(env, "exceptionText"), Napi::String::New(env, Helper::translateException(static_cast<SIMCONNECT_EXCEPTION>(message->dwException))));
    object.Set(Napi::String::New(env, "sendId"), Napi::Number::New(env, message->dwSendID));
    object.Set(Napi::String::New(env, "index"), Napi::Number::New(env, message->dwIndex));

    return object;
}

Napi::Value Dispatcher::requestClientData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->isConnected() == false) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() != 3) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'clientDataArea' must be an object of type ClientDataArea").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'period' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[2].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'flag' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto clientDataArea = Napi::ObjectWrap<ClientDataArea>::Unwrap(info[0].As<Napi::Object>());
    const auto period = static_cast<SIMCONNECT_CLIENT_DATA_PERIOD>(info[1].As<Napi::Number>().Uint32Value());
    const auto flag = static_cast<SIMCONNECT_DATA_REQUEST_FLAG>(info[2].As<Napi::Number>().Uint32Value());

    /* check if area is already in -> update flags, etc */
    for (auto area : this->_requestedClientAreas) {
        if (area->id() == clientDataArea->id()) {
            auto retval = area->updateRequestData(period, flag);
            return Napi::Boolean::New(env, retval);
        }
    }

    auto retval = clientDataArea->requestData(this->_requestId, period, flag);
    if (retval) {
        this->_requestedClientAreas.push_back(clientDataArea);
    }

    return Napi::Boolean::New(env, retval);
}

#include <iostream>
Napi::Object Dispatcher::convertClientDataAreaMessage(Napi::Env env, SIMCONNECT_RECV_CLIENT_DATA* message,
                                                      SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                                      const ClientDataArea::ClientDataDefinition& definition) {
    Napi::Object object = Napi::Object::New(env);

    object.Set(Napi::String::New(env, "clientDataId"), Napi::Number::New(env, clientDataId));

    switch (definition.sizeOrType) {
    case SIMCONNECT_CLIENTDATATYPE_INT8:
    case SIMCONNECT_CLIENTDATATYPE_INT16:
    case SIMCONNECT_CLIENTDATATYPE_INT32:
    case SIMCONNECT_CLIENTDATATYPE_INT64: {
        std::int64_t* array = reinterpret_cast<std::int64_t*>(&message->dwData);
        object.Set(Napi::String::New(env, definition.memberName), Napi::Number::New(env, array[0]));
        break;
    }
    case SIMCONNECT_CLIENTDATATYPE_FLOAT32:
    case SIMCONNECT_CLIENTDATATYPE_FLOAT64: {
        double* array = reinterpret_cast<double*>(&message->dwData);
        object.Set(Napi::String::New(env, definition.memberName), Napi::Number::New(env, array[0]));
        break;
    }
    default:
        std::cout << "SIZE: " << message->dwDefineCount * 8 << std::endl;
    }

    return object;
}

Napi::Value Dispatcher::nextDispatch(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->simConnect() == 0) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        this->_requestedClientAreas.clear();
        return env.Null();
    }

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Parameters are not allowed").ThrowAsJavaScriptException();
        return env.Null();
    }

    SIMCONNECT_RECV* receiveData;
    DWORD size;

    HRESULT result = SimConnect_GetNextDispatch(this->_connection->simConnect(), &receiveData, &size);
    if (result != S_OK) {
        return env.Null();
    }

    Napi::Object object = Napi::Object::New(env);
    bool validObject = true;

    switch (receiveData->dwID) {
    case SIMCONNECT_RECV_ID_OPEN:
        object.Set(Napi::String::New(env, "data"), Dispatcher::convertOpenMessage(env, static_cast<SIMCONNECT_RECV_OPEN*>(receiveData)));
        object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "open"));
        this->_connection->connectionEstablished(true);
        break;
    case SIMCONNECT_RECV_ID_QUIT:
        object.Set(Napi::String::New(env, "data"), Napi::Object::New(env));
        object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "quit"));
        this->_connection->connectionEstablished(false);
        break;
    case SIMCONNECT_RECV_ID_EXCEPTION:
        object.Set(Napi::String::New(env, "data"), Dispatcher::convertExceptionMessage(env, static_cast<SIMCONNECT_RECV_EXCEPTION*>(receiveData)));
        object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "exception"));
        break;
    case SIMCONNECT_RECV_ID_CLIENT_DATA: {
        SIMCONNECT_RECV_CLIENT_DATA* data = static_cast<SIMCONNECT_RECV_CLIENT_DATA*>(receiveData);

        bool found = false;
        for (const auto& area : std::as_const(this->_requestedClientAreas)) {
            for (const auto& definition : std::as_const(area->definitions())) {
                if (definition.requestId == data->dwRequestID) {
                    object.Set(Napi::String::New(env, "data"), Dispatcher::convertClientDataAreaMessage(env, data, area->id(), definition));
                    object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "clientData"));
                    found = true;
                    break;
                }
            }

            if (true == found) {
                break;
            }
        }

        if (false == found) {
            return env.Null();
        }
        break;
    }
    default:
        object.Set(Napi::String::New(env, "data"), Dispatcher::convertUnknownMessage(env, receiveData));
        object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "error"));
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
