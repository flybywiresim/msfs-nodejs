#include "context.h"

using namespace msfs::nodejs::simconnect;

Context::Context(v8::Isolate* isolate) {
    node::AddEnvironmentCleanupHook(isolate, &Context::deleteInstance, this);
}

Context::~Context() {
    this->close();
}

ReturnCode Context::open(v8::Isolate* isolate, v8::Local<v8::String> name) {
    if (this->hSimConnect == 0) {
        HRESULT result = SimConnect_Open(&this->hSimConnect, std::string(*v8::String::Utf8Value(isolate, name)).c_str(), nullptr, 0, 0, 0);
        if (result == S_OK) {
            return ReturnCode::Ok;
        } else if (result == E_FAIL) {
            return ReturnCode::Failure;
        } else {
            return ReturnCode::InvalidArgument;
        }
    }

    return ReturnCode::AlreadyConnected;
}

ReturnCode Context::close() {
    if (this->hSimConnect != 0) {
        SimConnect_Close(this->hSimConnect);
        this->hSimConnect = 0;
        return ReturnCode::Ok;
    }

    return ReturnCode::NotConnected;
}

bool Context::connected() const {
    return this->hSimConnect != 0;
}

ReturnCode Context::mapClientDataNameToId(v8::Isolate* isolate, v8::Local<v8::String> name, int dataId) {
    if (this->hSimConnect == 0) {
        return ReturnCode::NotConnected;
    }

    HRESULT result = SimConnect_MapClientDataNameToID(&this->hSimConnect, std::string(*v8::String::Utf8Value(isolate, name)).c_str(), dataId);
    if (result == E_FAIL) {
        this->close();
        return ReturnCode::Failure;
    } else if (result == SIMCONNECT_EXCEPTION_ALREADY_CREATED) {
        return ReturnCode::AlreadyCreated;
    } else if (result == SIMCONNECT_EXCEPTION_DUPLICATE_ID) {
        return ReturnCode::DuplicateId;
    }

    return ReturnCode::Ok;
}

void Context::deleteInstance(void* data) {
    delete static_cast<Context*>(data);
}