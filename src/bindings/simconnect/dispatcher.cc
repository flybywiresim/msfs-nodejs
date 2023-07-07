#include "dispatcher.h"
#include "helper.h"
#include "instancedata.h"

using namespace msfs::simconnect;

Dispatcher::Dispatcher(const Napi::CallbackInfo& info) :
        Napi::ObjectWrap<Dispatcher>(info),
        _connection(nullptr),
        _requestedClientAreas(),
        _requestedSimulatorDataArea(),
        _subscribedSystemEvents(),
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

Napi::Value Dispatcher::requestSimulatorData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->isConnected() == false) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Invalid argument type. 'simulatorDataArea' must be an object of type SimulatorDataArea").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Invalid argument type. 'period' must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto simulatorDataArea = Napi::ObjectWrap<SimulatorDataArea>::Unwrap(info[0].As<Napi::Object>());
    const auto period = static_cast<SIMCONNECT_PERIOD>(info[1].As<Napi::Number>().Uint32Value());

    /* check if area is already in -> update flags, etc */
    for (auto area : this->_requestedSimulatorDataArea) {
        if (area->id() == simulatorDataArea->id()) {
            auto retval = area->updateRequestData(period);
            return Napi::Boolean::New(env, retval);
        }
    }

    auto retval = simulatorDataArea->requestData(this->_requestId++, period);
    if (retval) {
        this->_requestedSimulatorDataArea.push_back(simulatorDataArea);
    }

    return Napi::Boolean::New(env, retval);
}

Napi::Value Dispatcher::subscribeSystemEvent(const Napi::CallbackInfo& info) {
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
        Napi::TypeError::New(env, "Invalid argument type. 'systemEvent' must be an object of type SystemEvent").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto systemEvent = Napi::ObjectWrap<SystemEvent>::Unwrap(info[0].As<Napi::Object>());

    for (auto area : this->_subscribedSystemEvents) {
        if (area->id() == systemEvent->id()) {
            return Napi::Boolean::New(env, true);
        }
    }

    auto retval = systemEvent->subscribe();
    if (retval) {
        this->_subscribedSystemEvents.push_back(systemEvent);
    }

    return Napi::Boolean::New(env, retval);
}

Napi::Value Dispatcher::unsubscribeSystemEvent(const Napi::CallbackInfo& info) {
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
        Napi::TypeError::New(env, "Invalid argument type. 'systemEvent' must be an object of type SystemEvent").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto systemEvent = Napi::ObjectWrap<SystemEvent>::Unwrap(info[0].As<Napi::Object>());

    for (auto it = this->_subscribedSystemEvents.begin(); it != this->_subscribedSystemEvents.end(); ++it) {
        if ((*it)->id() == systemEvent->id()) {
            auto retval = systemEvent->unsubscribe();
            if (true == retval) {
                this->_subscribedSystemEvents.erase(it);
            }
            return Napi::Boolean::New(env, retval);
        }
    }

    return Napi::Boolean::New(env, false);
}

Napi::Object Dispatcher::convertClientDataAreaMessage(Napi::Env env, SIMCONNECT_RECV_CLIENT_DATA* message,
                                                      SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                                      const ClientDataArea::ClientDataDefinition& definition) {
    Napi::Object object = Napi::Object::New(env);

    object.Set(Napi::String::New(env, "clientDataId"), Napi::Number::New(env, clientDataId));
    Napi::Object content = Napi::Object::New(env);
    object.Set(Napi::String::New(env, "content"), content);

    switch (definition.sizeOrType) {
    case SIMCONNECT_CLIENTDATATYPE_INT8: {
        std::int8_t* array = reinterpret_cast<std::int8_t*>(&message->dwData);
        content.Set(Napi::String::New(env, definition.memberName), Napi::Number::New(env, array[0]));
        break;
    }
    case SIMCONNECT_CLIENTDATATYPE_INT16: {
        std::int16_t* array = reinterpret_cast<std::int16_t*>(&message->dwData);
        content.Set(Napi::String::New(env, definition.memberName), Napi::Number::New(env, array[0]));
        break;
    }
    case SIMCONNECT_CLIENTDATATYPE_INT32: {
        std::int32_t* array = reinterpret_cast<std::int32_t*>(&message->dwData);
        content.Set(Napi::String::New(env, definition.memberName), Napi::Number::New(env, array[0]));
        break;
    }
    case SIMCONNECT_CLIENTDATATYPE_INT64: {
        std::int64_t* array = reinterpret_cast<std::int64_t*>(&message->dwData);
        content.Set(Napi::String::New(env, definition.memberName), Napi::Number::New(env, array[0]));
        break;
    }
    case SIMCONNECT_CLIENTDATATYPE_FLOAT32: {
        float* array = reinterpret_cast<float*>(&message->dwData);
        content.Set(Napi::String::New(env, definition.memberName), Napi::Number::New(env, array[0]));
        break;
    }
    case SIMCONNECT_CLIENTDATATYPE_FLOAT64: {
        double* array = reinterpret_cast<double*>(&message->dwData);
        content.Set(Napi::String::New(env, definition.memberName), Napi::Number::New(env, array[0]));
        break;
    }
    default: {
        Napi::ArrayBuffer output = Napi::ArrayBuffer::New(env, reinterpret_cast<void*>(&message->dwData), definition.sizeOrType);
        content.Set(Napi::String::New(env, definition.memberName), output);
        break;
    }
    }

    return object;
}

Napi::Object Dispatcher::convertSimulatorDataInitPosition(Napi::Env env, std::uint8_t* data) {
    Napi::Object object = Napi::Object::New(env);

    SIMCONNECT_DATA_INITPOSITION* initPosition = (SIMCONNECT_DATA_INITPOSITION*)data;

    object.Set(Napi::String::New(env, "latitude"), Napi::Number::New(env, initPosition->Latitude));
    object.Set(Napi::String::New(env, "longitude"), Napi::Number::New(env, initPosition->Longitude));
    object.Set(Napi::String::New(env, "altitude"), Napi::Number::New(env, initPosition->Altitude));
    object.Set(Napi::String::New(env, "pitch"), Napi::Number::New(env, initPosition->Pitch));
    object.Set(Napi::String::New(env, "bank"), Napi::Number::New(env, initPosition->Bank));
    object.Set(Napi::String::New(env, "heading"), Napi::Number::New(env, initPosition->Heading));
    object.Set(Napi::String::New(env, "onGround"), Napi::Boolean::New(env, initPosition->OnGround == 1));
    object.Set(Napi::String::New(env, "airspeed"), Napi::Number::New(env, initPosition->Airspeed));

    return object;
}

Napi::Object Dispatcher::convertSimulatorDataMarkerState(Napi::Env env, std::uint8_t* data) {
    Napi::Object object = Napi::Object::New(env);

    SIMCONNECT_DATA_MARKERSTATE* state = (SIMCONNECT_DATA_MARKERSTATE*)data;

    object.Set(Napi::String::New(env, "name"), Napi::String::New(env, state->szMarkerName));
    object.Set(Napi::String::New(env, "isOn"), Napi::Boolean::New(env, state->dwMarkerState == 1));

    return object;
}

Napi::Object Dispatcher::convertSimulatorDataWaypoint(Napi::Env env, std::uint8_t* data) {
    Napi::Object object = Napi::Object::New(env);

    SIMCONNECT_DATA_WAYPOINT* waypoint = (SIMCONNECT_DATA_WAYPOINT*)data;

    object.Set(Napi::String::New(env, "latitude"), Napi::Number::New(env, waypoint->Latitude));
    object.Set(Napi::String::New(env, "longitude"), Napi::Number::New(env, waypoint->Longitude));
    object.Set(Napi::String::New(env, "altitude"), Napi::Number::New(env, waypoint->Altitude));
    object.Set(Napi::String::New(env, "flags"), Napi::Number::New(env, waypoint->Flags));
    object.Set(Napi::String::New(env, "speedInKnots"), Napi::Number::New(env, waypoint->ktsSpeed));
    if (waypoint->Flags & SIMCONNECT_WAYPOINT_THROTTLE_REQUESTED) {
        object.Set(Napi::String::New(env, "percentThrottle"), Napi::Number::New(env, waypoint->percentThrottle));
    }

    return object;
}

Napi::Object Dispatcher::convertSimulatorDataLatLongAlt(Napi::Env env, std::uint8_t* data) {
    Napi::Object object = Napi::Object::New(env);

    SIMCONNECT_DATA_LATLONALT* position = (SIMCONNECT_DATA_LATLONALT*)data;

    object.Set(Napi::String::New(env, "latitude"), Napi::Number::New(env, position->Latitude));
    object.Set(Napi::String::New(env, "longitude"), Napi::Number::New(env, position->Longitude));
    object.Set(Napi::String::New(env, "altitude"), Napi::Number::New(env, position->Altitude));

    return object;
}

Napi::Object Dispatcher::convertSimulatorDataXYZ(Napi::Env env, std::uint8_t* data) {
    Napi::Object object = Napi::Object::New(env);

    SIMCONNECT_DATA_XYZ* xyz = (SIMCONNECT_DATA_XYZ*)data;

    object.Set(Napi::String::New(env, "x"), Napi::Number::New(env, xyz->x));
    object.Set(Napi::String::New(env, "y"), Napi::Number::New(env, xyz->y));
    object.Set(Napi::String::New(env, "z"), Napi::Number::New(env, xyz->z));

    return object;
}

Napi::Object Dispatcher::convertSimulatorDataArea(Napi::Env env, SIMCONNECT_RECV* receivedData, DWORD sizeReceivedData, std::uint8_t* data,
                                                  const std::list<SimulatorDataArea::SimulatorDataDefinition>& definition) {
    Napi::Object object = Napi::Object::New(env);
    std::size_t offset = 0;

    for (const auto& entry : std::as_const(definition)) {
        switch (entry.type) {
        case SIMCONNECT_DATATYPE_FLOAT32:
            object.Set(Napi::String::New(env, entry.memberName), Napi::Number::New(env, *((float*)&data[offset])));
            offset += sizeof(float);
            break;
        case SIMCONNECT_DATATYPE_FLOAT64:
            object.Set(Napi::String::New(env, entry.memberName), Napi::Number::New(env, *((double*)&data[offset])));
            offset += sizeof(double);
            break;
        case SIMCONNECT_DATATYPE_INT32:
            object.Set(Napi::String::New(env, entry.memberName), Napi::Number::New(env, *((std::int32_t*)&data[offset])));
            offset += sizeof(std::int32_t);
            break;
        case SIMCONNECT_DATATYPE_INT64:
            object.Set(Napi::String::New(env, entry.memberName), Napi::Number::New(env, *((std::int64_t*)&data[offset])));
            offset += sizeof(std::int64_t);
            break;
        case SIMCONNECT_DATATYPE_STRING8:
            object.Set(Napi::String::New(env, entry.memberName), Napi::String::New(env, std::string((char*)&data[offset], (char*)&data[offset + 8])));
            offset += 8;
            break;
        case SIMCONNECT_DATATYPE_STRING32:
            object.Set(Napi::String::New(env, entry.memberName), Napi::String::New(env, std::string((char*)&data[offset], (char*)&data[offset + 32])));
            offset += 32;
            break;
        case SIMCONNECT_DATATYPE_STRING64:
            object.Set(Napi::String::New(env, entry.memberName), Napi::String::New(env, std::string((char*)&data[offset], (char*)&data[offset + 64])));
            offset += 64;
            break;
        case SIMCONNECT_DATATYPE_STRING128:
            object.Set(Napi::String::New(env, entry.memberName), Napi::String::New(env, std::string((char*)&data[offset], (char*)&data[offset + 128])));
            offset += 128;
            break;
        case SIMCONNECT_DATATYPE_STRING256:
            object.Set(Napi::String::New(env, entry.memberName), Napi::String::New(env, std::string((char*)&data[offset], (char*)&data[offset + 256])));
            offset += 256;
            break;
        case SIMCONNECT_DATATYPE_STRING260:
            object.Set(Napi::String::New(env, entry.memberName), Napi::String::New(env, std::string((char*)&data[offset], (char*)&data[offset + 260])));
            offset += 260;
            break;
        case SIMCONNECT_DATATYPE_STRINGV: {
            char* string;
            DWORD length;

            HRESULT result = SimConnect_RetrieveString(receivedData, sizeReceivedData, (BYTE*)&data[offset], &string, &length);
            if (result != S_OK) {
                this->_lastError = "Unable to decode variable string of " + entry.memberName;
                return Napi::Object::New(env);
            }

            object.Set(Napi::String::New(env, entry.memberName), Napi::String::New(env, string));
            offset += length;

            break;
        }
        case SIMCONNECT_DATATYPE_INITPOSITION:
            object.Set(Napi::String::New(env, entry.memberName), Dispatcher::convertSimulatorDataInitPosition(env, &data[offset]));
            offset += sizeof(SIMCONNECT_DATA_INITPOSITION);
            break;
        case SIMCONNECT_DATATYPE_MARKERSTATE:
            object.Set(Napi::String::New(env, entry.memberName), Dispatcher::convertSimulatorDataMarkerState(env, &data[offset]));
            offset += sizeof(SIMCONNECT_DATA_MARKERSTATE);
            break;
        case SIMCONNECT_DATATYPE_WAYPOINT:
            object.Set(Napi::String::New(env, entry.memberName), Dispatcher::convertSimulatorDataWaypoint(env, &data[offset]));
            offset += sizeof(SIMCONNECT_DATA_WAYPOINT);
            break;
        case SIMCONNECT_DATATYPE_LATLONALT:
            object.Set(Napi::String::New(env, entry.memberName), Dispatcher::convertSimulatorDataLatLongAlt(env, &data[offset]));
            offset += sizeof(SIMCONNECT_DATA_LATLONALT);
            break;
        default:
            object.Set(Napi::String::New(env, entry.memberName), Dispatcher::convertSimulatorDataXYZ(env, &data[offset]));
            offset += sizeof(SIMCONNECT_DATA_XYZ);
            break;
        }
    }

    return object;
}

bool Dispatcher::processFilenameSystemEvents(Napi::Env env, SIMCONNECT_RECV* receivedData, const SystemEvent* event, Napi::Object& returnObject) {
    static const std::vector<std::string> filenameEvents{
        "AircraftLoaded",
        "FlightLoaded",
        "FlightSaved",
        "FlightPlanActivated",
    };

    auto foundEventName = std::find(std::cbegin(filenameEvents), std::cend(filenameEvents), event->eventName()) != std::cend(filenameEvents);
    if (foundEventName == true) {
        SIMCONNECT_RECV_EVENT_FILENAME* data = static_cast<SIMCONNECT_RECV_EVENT_FILENAME*>(receivedData);
        Napi::Object filenameObject = Napi::Object::New(env);
        filenameObject.Set(Napi::String::New(env, "filename"), Napi::String::New(env, data->szFileName));
        returnObject.Set(Napi::String::New(env, "content"), filenameObject);
    }

    return foundEventName;
}

bool Dispatcher::processObjectSystemEvents(Napi::Env env, SIMCONNECT_RECV* receivedData, const SystemEvent* event, Napi::Object& returnObject) {
    static const std::vector<std::string> objectEvents{
        "ObjectAdded",
        "ObjectRemoved",
    };

    auto foundEventName = std::find(std::cbegin(objectEvents), std::cend(objectEvents), event->eventName()) != std::cend(objectEvents);
    if (foundEventName == true) {
        SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE* data = static_cast<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE*>(receivedData);
        Napi::Object aiObject = Napi::Object::New(env);

        aiObject.Set(Napi::String::New(env, "type"), Napi::Number::New(env, data->eObjType));
        aiObject.Set(Napi::String::New(env, "id"), Napi::Number::New(env, data->dwData));
        returnObject.Set(Napi::String::New(env, "content"), aiObject);
    }

    return foundEventName;
}

Napi::Value Dispatcher::nextDispatch(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (this->_connection->simConnect() == 0) {
        Napi::Error::New(env, "Not connected to the server").ThrowAsJavaScriptException();
        this->_requestedClientAreas.clear();
        this->_requestedSimulatorDataArea.clear();
        this->_subscribedSystemEvents.clear();
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
    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
        SIMCONNECT_RECV_SIMOBJECT_DATA* data = static_cast<SIMCONNECT_RECV_SIMOBJECT_DATA*>(receiveData);

        bool found = false;
        for (const auto& area : std::as_const(this->_requestedSimulatorDataArea)) {
            if (area->id() == data->dwRequestID) {
                std::uint8_t* array = (std::uint8_t*)&data->dwData;

                Napi::Object dataObject = Napi::Object::New(env);
                dataObject.Set(Napi::String::New(env, "definitionId"), Napi::Number::New(env, area->id()));
                Napi::Object content = Napi::Object::New(env);
                dataObject.Set(Napi::String::New(env, "content"), this->convertSimulatorDataArea(env, receiveData, size, array, area->dataDefinitions()));

                object.Set(Napi::String::New(env, "data"), dataObject);
                object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "simulatorData"));

                found = true;
                break;
            }
        }

        if (false == found) {
            return env.Null();
        }

        break;
    }
    // all system event messages
    case SIMCONNECT_RECV_ID_EVENT:
    case SIMCONNECT_RECV_ID_EVENT_FILENAME:
    case SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE: {
        SIMCONNECT_RECV_EVENT* data = static_cast<SIMCONNECT_RECV_EVENT*>(receiveData);
        for (const auto& event : std::as_const(this->_subscribedSystemEvents)) {
            if (event->id() == data->uEventID) {
                Napi::Object eventObject = Napi::Object::New(env);
                eventObject.Set(Napi::String::New(env, "eventId"), Napi::Number::New(env, event->id()));

                if (event->eventName() == "Pause" || event->eventName() == "Pause_EX1") {
                    Napi::Object pauseObject = Napi::Object::New(env);
                    pauseObject.Set(Napi::String::New(env, "type"), Napi::Number::New(env, data->dwData));
                    eventObject.Set(Napi::String::New(env, "content"), pauseObject);
                } else if (event->eventName() == "Sim") {
                    Napi::Object simObject = Napi::Object::New(env);
                    simObject.Set(Napi::String::New(env, "running"), Napi::Boolean::New(env, data->dwData == 1));
                    eventObject.Set(Napi::String::New(env, "content"), simObject);
                } else if (event->eventName() == "Sound") {
                    Napi::Object soundObject = Napi::Object::New(env);
                    soundObject.Set(Napi::String::New(env, "soundSwitchOn"), Napi::Boolean::New(env, data->dwData != 0));
                    eventObject.Set(Napi::String::New(env, "content"), soundObject);
                } else if (event->eventName() == "View") {
                    Napi::Object viewObject = Napi::Object::New(env);
                    viewObject.Set(Napi::String::New(env, "view"), Napi::Number::New(env, data->dwData));
                    eventObject.Set(Napi::String::New(env, "content"), viewObject);
                } else if (false == Dispatcher::processFilenameSystemEvents(env, receiveData, event, eventObject)) {
                    Dispatcher::processObjectSystemEvents(env, receiveData, event, eventObject);
                }

                object.Set(Napi::String::New(env, "type"), Napi::String::New(env, "systemEvent"));
                object.Set(Napi::String::New(env, "data"), eventObject);
            }
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
        InstanceMethod<&Dispatcher::requestClientData>("requestClientData", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Dispatcher::requestSimulatorData>("requestSimulatorData", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Dispatcher::subscribeSystemEvent>("subscribeSystemEvent", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Dispatcher::unsubscribeSystemEvent>("unsubscribeSystemEvent", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Dispatcher::nextDispatch>("nextDispatch", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Dispatcher::lastError>("lastError", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.GetInstanceData<InstanceData>()->dispatcherConstructor = constructor;

    exports.Set("DispatcherBindings", func);
    return exports;
}
