#include "helper.h"

using namespace msfs::simconnect;

std::string Helper::translateException(SIMCONNECT_EXCEPTION exception) {
    switch (exception) {
        case SIMCONNECT_EXCEPTION_NONE:
            return "";
        case SIMCONNECT_EXCEPTION_ERROR:
            return "Unspecified error";
        case SIMCONNECT_EXCEPTION_SIZE_MISMATCH:
          return "Sizes do not match";
        case SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID:
            return "Unrecognized ID";
        case SIMCONNECT_EXCEPTION_UNOPENED:
            return "Unopened";
        case SIMCONNECT_EXCEPTION_VERSION_MISMATCH:
            return "Version mismatch";
        case SIMCONNECT_EXCEPTION_TOO_MANY_GROUPS:
            return "Too many groups";
        case SIMCONNECT_EXCEPTION_NAME_UNRECOGNIZED:
            return "Name unrecognized";
        case SIMCONNECT_EXCEPTION_TOO_MANY_EVENT_NAMES:
            return "Too many event names";
        case SIMCONNECT_EXCEPTION_EVENT_ID_DUPLICATE:
            return "Event ID duplicate";
        case SIMCONNECT_EXCEPTION_TOO_MANY_MAPS:
            return "Too many maps";
        case SIMCONNECT_EXCEPTION_TOO_MANY_OBJECTS:
            return "Too many objects";
        case SIMCONNECT_EXCEPTION_TOO_MANY_REQUESTS:
            return "Too many request";
        case SIMCONNECT_EXCEPTION_WEATHER_INVALID_PORT:
            return "Weather invalid port";
        case SIMCONNECT_EXCEPTION_WEATHER_INVALID_METAR:
            return "Weather invalid METAR";
        case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_GET_OBSERVATION:
            return "Weather unable to get observation";
        case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_CREATE_STATION:
            return "Weather unable to create station";
        case SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_REMOVE_STATION:
            return "Weather unable to remove station";
        case SIMCONNECT_EXCEPTION_INVALID_DATA_TYPE:
            return "Invalid data type";
        case SIMCONNECT_EXCEPTION_INVALID_DATA_SIZE:
            return "Invalid data size";
        case SIMCONNECT_EXCEPTION_DATA_ERROR:
            return "Data error";
        case SIMCONNECT_EXCEPTION_INVALID_ARRAY:
            return "Invalid array";
        case SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED:
            return "Create object failed";
        case SIMCONNECT_EXCEPTION_LOAD_FLIGHTPLAN_FAILED:
            return "Load flightplan failed";
        case SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE:
            return "Operation invalid for object type";
        case SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION:
            return "Illegal operation";
        case SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED:
            return "Already subscribed";
        case SIMCONNECT_EXCEPTION_INVALID_ENUM:
            return "Invalid enumeration";
        case SIMCONNECT_EXCEPTION_DEFINITION_ERROR:
            return "Definition error";
        case SIMCONNECT_EXCEPTION_DUPLICATE_ID:
            return "Duplicate ID";
        case SIMCONNECT_EXCEPTION_DATUM_ID:
            return "Datum ID";
        case SIMCONNECT_EXCEPTION_OUT_OF_BOUNDS:
            return "Out of bounds";
        case SIMCONNECT_EXCEPTION_ALREADY_CREATED:
            return "Already created";
        case SIMCONNECT_EXCEPTION_OBJECT_OUTSIDE_REALITY_BUBBLE:
            return "Object outside reality bubble";
        case SIMCONNECT_EXCEPTION_OBJECT_CONTAINER:
            return "Object container";
        case SIMCONNECT_EXCEPTION_OBJECT_AI:
            return "Object AI";
        case SIMCONNECT_EXCEPTION_OBJECT_ATC:
            return "Object ATC";
        case SIMCONNECT_EXCEPTION_OBJECT_SCHEDULE:
            return "Object schedule";
        default:
            return "Unknown exception";
    }
}
