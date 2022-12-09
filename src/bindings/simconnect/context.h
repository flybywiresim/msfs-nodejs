#pragma once

#include <Windows.h>
#include <SimConnect.h>
#include <node.h>

namespace msfs {
namespace nodejs {
namespace simconnect {
    class Context {
        private:
            HANDLE hSimConnect = 0;
            std::string sLastError = "";

        public:
            explicit Context(v8::Isolate* isolate);
            ~Context();

            bool open(v8::Isolate* isolate, v8::Local<v8::String> name);
            bool close();
            bool connected() const;
            bool mapClientDataNameToId(v8::Isolate* isolate, v8::Local<v8::String> name, int dataId);
            bool createClientData(int dataId, DWORD datasize, bool readOnly);
            bool addToClientDataDefinition(int dataDefinitionId, int offset, int sizeOrType, float epsilon);
            bool setClientData(int dataId, int dataDefinitionId, DWORD size, char* data);
            const std::string& lastError() const;

            static void deleteInstance(void* data);
    };
}
}
}
