#include <node.h>

#include "clientdatadefinition.h"
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
                 FunctionTemplate::New(isolate, ClientDataDefinition::mapClientDataNameToId, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "createClientData").ToLocalChecked(),
                 FunctionTemplate::New(isolate, ClientDataDefinition::createClientData, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "addToClientDataDefinition").ToLocalChecked(),
                 FunctionTemplate::New(isolate, ClientDataDefinition::addToClientDataDefinition, external)->GetFunction(context).ToLocalChecked()).FromJust();
}

// NODE_MODULE_CONTEXT_AWARE does not work with VS2019
NODE_MODULE_CONTEXT_AWARE_X(NODE_GYP_MODULE_NAME, Initialize, NULL, 0)
