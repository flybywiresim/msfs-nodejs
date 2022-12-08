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
}

void ClientDataDefinition::addToClientDataDefinition(const FunctionCallbackInfo<Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    if (!context->connected()) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Not connected to the server").ToLocalChecked()));
    }
}
