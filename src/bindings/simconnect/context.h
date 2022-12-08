#pragma once

#include <Windows.h>
#include <SimConnect.h>
#include <node.h>

#include "returncode.h"

namespace msfs {
namespace nodejs {
namespace simconnect {
    class Context {
        private:
            HANDLE hSimConnect = 0;

        public:
            explicit Context(v8::Isolate* isolate);
            ~Context();

            ReturnCode open(v8::Isolate* isolate, v8::Local<v8::String> name);
            ReturnCode close();
            bool connected() const;
            ReturnCode mapClientDataNameToId(v8::Isolate* isolate, v8::Local<v8::String> name, int dataId);

            static void deleteInstance(void* data);
    };
}
}
}
