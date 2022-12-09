#include <node.h>

#include "clientdatadefinition.h"
#include "connection.h"
#include "context.h"

using namespace v8;
using namespace msfs::nodejs;
using namespace msfs::nodejs::simconnect;

void Initialize(Local<Object> exports, Local<Module> module, Local<v8::Context> context) {
    Isolate* isolate = context->GetIsolate();

    msfs::nodejs::simconnect::Context* data = new msfs::nodejs::simconnect::Context(isolate);

    Local<External> external = External::New(isolate, data);

    exports->Set(context,
                 String::NewFromUtf8(isolate, "openSimConnect").ToLocalChecked(),
                 FunctionTemplate::New(isolate, Connection::open, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "closeSimConnect").ToLocalChecked(),
                 FunctionTemplate::New(isolate, Connection::close, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "mapClientDataNameToId").ToLocalChecked(),
                 FunctionTemplate::New(isolate, ClientDataDefinition::mapClientDataNameToId, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "createClientData").ToLocalChecked(),
                 FunctionTemplate::New(isolate, ClientDataDefinition::createClientData, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "addToClientDataDefinition").ToLocalChecked(),
                 FunctionTemplate::New(isolate, ClientDataDefinition::addToClientDataDefinition, external)->GetFunction(context).ToLocalChecked()).FromJust();
    exports->Set(context,
                 String::NewFromUtf8(isolate, "setClientData").ToLocalChecked(),
                 FunctionTemplate::New(isolate, ClientDataDefinition::setClientData, external)->GetFunction(context).ToLocalChecked()).FromJust();
}

// NODE_MODULE_CONTEXT_AWARE does not work with VS2019
NODE_MODULE_CONTEXT_AWARE_X(NODE_GYP_MODULE_NAME, Initialize, NULL, 0)
