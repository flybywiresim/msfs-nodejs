#pragma once

#include <node.h>

namespace msfs {
namespace nodejs {
namespace simconnect {
    class ClientDataDefinition {
        public:
            static void mapClientDataNameToId(const v8::FunctionCallbackInfo<v8::Value>& info);
            static void createClientData(const v8::FunctionCallbackInfo<v8::Value>& info);
            static void addToClientDataDefinition(const v8::FunctionCallbackInfo<v8::Value>& info);
    };
}
}
}
