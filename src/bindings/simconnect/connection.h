#pragma once

#include <node.h>

namespace msfs {
namespace nodejs {
namespace simconnect {
    class Connection {
        public:
            static void open(const v8::FunctionCallbackInfo<v8::Value>& info);
            static void close(const v8::FunctionCallbackInfo<v8::Value>& info);
    };
}
}
}
