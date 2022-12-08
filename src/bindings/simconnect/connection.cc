#include "connection.h"
#include "context.h"

using namespace v8;
using namespace msfs::nodejs;
using namespace msfs::nodejs::simconnect;

void Connection::open(const v8::FunctionCallbackInfo<v8::Value>& info) {
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

void Connection::close(const v8::FunctionCallbackInfo<v8::Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    Isolate* isolate = info.GetIsolate();

    auto returncode = context->close();
    if (returncode == ReturnCode::NotConnected) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Were not connected to the server").ToLocalChecked()));
    }
}
