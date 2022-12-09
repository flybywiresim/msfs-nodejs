#include <node_buffer.h>

#include "clientdatadefinition.h"
#include "context.h"

using namespace v8;
using namespace msfs::nodejs;
using namespace msfs::nodejs::simconnect;

void ClientDataDefinition::mapClientDataNameToId(const FunctionCallbackInfo<Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    if (!context->connected()) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Not connected to the server").ToLocalChecked()));
    }

    if (info.Length() != 2) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid number of arguments").ToLocalChecked()));
    }
    if (!info[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. Name must be a string").ToLocalChecked()));
    }
    if (!info[1]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. dataId must be a number").ToLocalChecked()));
    }

    Local<String> name = info[0].As<String>();
    if (name->Length() <= 0) {
        isolate->ThrowException(Exception::RangeError(String::NewFromUtf8(isolate, "Empty string is not allowed").ToLocalChecked()));
    }

    int dataId = static_cast<int>(info[1].As<Number>()->Value());

    auto returncode = context->mapClientDataNameToId(isolate, name, dataId);
    if (returncode == ReturnCode::Failure) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Unknown error occured. Connection closed").ToLocalChecked()));
    } else if (returncode == ReturnCode::AlreadyCreated) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Mapping already created").ToLocalChecked()));
    } else if (returncode == ReturnCode::DuplicateId) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Found duplicated ID").ToLocalChecked()));
    }
}

void ClientDataDefinition::createClientData(const FunctionCallbackInfo<Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    if (!context->connected()) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Not connected to the server").ToLocalChecked()));
    }

    if (info.Length() != 3) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid number of arguments").ToLocalChecked()));
    }
    if (!info[0]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. dataId must be a number").ToLocalChecked()));
    }
    if (!info[1]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. datasize must be a number").ToLocalChecked()));
    }
    if (!info[2]->IsBoolean()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. readOnly must be a boolean").ToLocalChecked()));
    }

    int dataId = static_cast<int>(info[0].As<Number>()->Value());
    DWORD datasize = static_cast<DWORD>(info[1].As<Number>()->Value());
    bool readOnly = info[2].As<Boolean>()->Value();

    auto returncode = context->createClientData(dataId, datasize, readOnly);
    if (returncode == ReturnCode::Failure) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Unknown error occured. Connection closed").ToLocalChecked()));
    } else if (returncode == ReturnCode::InvalidDataSize) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Datasize exceeds SIMCONNECT_MAX_SIZE").ToLocalChecked()));
    } else if (returncode == ReturnCode::OutOfBounds) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Datasize exceeds maximum memory").ToLocalChecked()));
    }
}

void ClientDataDefinition::addToClientDataDefinition(const FunctionCallbackInfo<Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    if (!context->connected()) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Not connected to the server").ToLocalChecked()));
    }

    if (info.Length() != 4) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid number of arguments").ToLocalChecked()));
    }
    if (!info[0]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. dataDefinitionId must be a number").ToLocalChecked()));
    }
    if (!info[1]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. offset must be a number").ToLocalChecked()));
    }
    if (!info[2]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. sizeOrType must be a number").ToLocalChecked()));
    }
    if (!info[3]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. epsilon must be a number").ToLocalChecked()));
    }

    int dataDefinitionId = static_cast<int>(info[0].As<Number>()->Value());
    int offset = static_cast<int>(info[1].As<Number>()->Value());
    int sizeOrType = static_cast<int>(info[2].As<Number>()->Value());
    float epsilon = info[3].As<Number>()->Value();

    auto returncode = context->addToClientDataDefinition(dataDefinitionId, offset, sizeOrType, epsilon);
    if (returncode == ReturnCode::Failure) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Unknown error occured. Connection closed").ToLocalChecked()));
    }
}

void ClientDataDefinition::setClientData(const v8::FunctionCallbackInfo<v8::Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    if (!context->connected()) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Not connected to the server").ToLocalChecked()));
    }

    if (info.Length() != 3) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid number of arguments").ToLocalChecked()));
    }
    if (!info[0]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. dataId must be a number").ToLocalChecked()));
    }
    if (!info[1]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. dataDefinitionId must be a number").ToLocalChecked()));
    }
    if (!info[2]->IsObject()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. data must be a buffer").ToLocalChecked()));
    }

    int dataId = static_cast<int>(info[0].As<Number>()->Value());
    int dataDefinitionId = static_cast<int>(info[1].As<Number>()->Value());
    char* data = node::Buffer::Data(info[2]);

    auto returncode = context->setClientData(dataId, dataDefinitionId, data);
    if (returncode == ReturnCode::Failure) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Unknown error occured. Connection closed").ToLocalChecked()));
    }
}
