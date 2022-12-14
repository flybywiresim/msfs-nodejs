#pragma once

#include <napi.h>

namespace msfs {
namespace simconnect {
    struct InstanceData {
        Napi::FunctionReference* connectionConstructor = nullptr;
        Napi::FunctionReference* clientDataAreaConstructor = nullptr;
        Napi::FunctionReference* simulatorDataAreaConstructor = nullptr;
        Napi::FunctionReference* dispatcherConstructor = nullptr;
    };
}
}
