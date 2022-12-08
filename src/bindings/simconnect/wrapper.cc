#include <node.h>

#include "context.h"

using namespace v8;
using namespace msfs::nodejs;
using namespace msfs::nodejs::simconnect;

void OpenSimConnect(const FunctionCallbackInfo<Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    if (info.Length() != 1) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid number of arguments").ToLocalChecked()));
    }
    if (!info[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Invalid argument type. Name must be a string").ToLocalChecked()));
    }

    Local<String> name = info[0].As<String>();
    if (name->Length() <= 0) {
        isolate->ThrowException(Exception::RangeError(String::NewFromUtf8(isolate, "Empty string is not allowed").ToLocalChecked()));
    }

    auto returncode = context->open(isolate, name);
    if (returncode == ReturnCode::Failure) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Unable to establish a connection").ToLocalChecked()));
    } else if (returncode == ReturnCode::InvalidArgument) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid connection name").ToLocalChecked()));
    } else if (returncode == ReturnCode::AlreadyConnected) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Already connected to the server").ToLocalChecked()));
    }
}

void CloseSimConnect(const FunctionCallbackInfo<Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    auto returncode = context->close();
    if (returncode == ReturnCode::NotConnected) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Were not connected to the server").ToLocalChecked()));
    }
}

void MapClientDataNameToId(const FunctionCallbackInfo<Value>& info) {
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

void CreateClientData(const FunctionCallbackInfo<Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    if (!context->connected()) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Not connected to the server").ToLocalChecked()));
    }
}

void AddToClientDataDefinition(const FunctionCallbackInfo<Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    if (!context->connected()) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Not connected to the server").ToLocalChecked()));
    }
}

void Initialize(Local<Object> exports, Local<Module> module, Local<v8::Context> context) {
    Isolate* isolate = context->GetIsolate();

    msfs::nodejs::simconnect::Context* data = new msfs::nodejs::simconnect::Context(isolate);

    Local<External> external = External::New(isolate, data);

    exports->Set(context,
                 String::NewFromUtf8(isolate, "openSimConnect").ToLocalChecked(),
                 FunctionTemplate::New(isolate, OpenSimConnect, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "closeSimConnect").ToLocalChecked(),
                 FunctionTemplate::New(isolate, CloseSimConnect, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "mapClientDataNameToId").ToLocalChecked(),
                 FunctionTemplate::New(isolate, MapClientDataNameToId, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "createClientData").ToLocalChecked(),
                 FunctionTemplate::New(isolate, CreateClientData, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "addToClientDataDefinition").ToLocalChecked(),
                 FunctionTemplate::New(isolate, AddToClientDataDefinition, external)->GetFunction(context).ToLocalChecked()).FromJust();
}

// NODE_MODULE_CONTEXT_AWARE does not work with VS2019
NODE_MODULE_CONTEXT_AWARE_X(NODE_GYP_MODULE_NAME, Initialize, NULL, 0)
