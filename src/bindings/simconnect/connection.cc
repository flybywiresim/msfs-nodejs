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

    auto success = context->open(isolate, name);
    if (!success) {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, context->lastError().c_str()).ToLocalChecked()));
    }
}

void Connection::close(const v8::FunctionCallbackInfo<v8::Value>& info) {
    simconnect::Context* context = reinterpret_cast<simconnect::Context*>(info.Data().As<External>()->Value());
    context->close();
}
