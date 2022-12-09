#include "context.h"
#include "helper.h"

using namespace msfs::nodejs::simconnect;

Context::Context(v8::Isolate* isolate) {
    node::AddEnvironmentCleanupHook(isolate, &Context::deleteInstance, this);
}

Context::~Context() {
    this->close();
}

bool Context::open(v8::Isolate* isolate, v8::Local<v8::String> name) {
    if (this->hSimConnect == 0) {
        HRESULT result = SimConnect_Open(&this->hSimConnect, std::string(*v8::String::Utf8Value(isolate, name)).c_str(), nullptr, 0, 0, 0);
        if (result != S_OK) {
            this->sLastError = Helper::translateException((SIMCONNECT_EXCEPTION)result);
            return false;
        }
    }

    return true;
}

bool Context::close() {
    if (this->hSimConnect != 0) {
        SimConnect_Close(this->hSimConnect);
        this->hSimConnect = 0;
    }

    return true;
}

bool Context::connected() const {
    return this->hSimConnect != 0;
}

bool Context::mapClientDataNameToId(v8::Isolate* isolate, v8::Local<v8::String> name, int dataId) {
    if (this->hSimConnect == 0) {
        this->sLastError = "Not connected";
        return false;
    }

    const auto nameStr = std::string(*v8::String::Utf8Value(isolate, name));
    HRESULT result = SimConnect_MapClientDataNameToID(&this->hSimConnect, nameStr.c_str(), dataId);
    if (result != E_FAIL) {
        if (result == E_FAIL)
            this->close();

        this->sLastError = Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return false;
    }

    return true;
}

ReturnCode Context::createClientData(int dataId, DWORD datasize, bool readOnly) {
    if (this->hSimConnect == 0) {
        this->sLastError = "Not connected";
        return false;
    }

    DWORD readOnlyFlag = readOnly ? SIMCONNECT_CREATE_CLIENT_DATA_FLAG_READ_ONLY : 0;
    HRESULT result = SimConnect_CreateClientData(this->hSimConnect, dataId, datasize, readOnlyFlag);

    if (result != S_OK) {
        if (result == E_FAIL)
            this->close();

        this->sLastError = Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return false;
    }

    return true;
}

bool Context::addToClientDataDefinition(int dataDefinitionId, int offset, int sizeOrType, float epsilon) {
    if (this->hSimConnect == 0) {
        this->sLastError = "Not connected";
        return false;
    }

    HRESULT result = SimConnect_AddToClientDataDefinition(this->hSimConnect, dataDefinitionId, offset, sizeOrType, epsilon);

    if (result != S_OK) {
        if (result == E_FAIL)
            this->close();

        this->sLastError = Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return false;
    }

    return true;
}

ReturnCode Context::setClientData(int dataId, int dataDefinitionId, DWORD size, char* data) {
    if (this->hSimConnect == 0) {
        this->sLastError = "Not connected";
        return false;
    }

    HRESULT result = SimConnect_SetClientData(this->hSimConnect, dataId, dataDefinitionId, 0, 0, size, data);

    if (result != S_OK) {
        if (result == E_FAIL)
            this->close();

        this->sLastError = Helper::translateException((SIMCONNECT_EXCEPTION)result);
        return false;
    }

    return true;
}

const std::string& Context::lastError() const {
    return this->sLastError;
}

void Context::deleteInstance(void* data) {
    delete static_cast<Context*>(data);
}
