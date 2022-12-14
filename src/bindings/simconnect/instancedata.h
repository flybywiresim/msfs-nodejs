#pragma once

#include <napi.h>

namespace msfs {
namespace simconnect {
    struct InstanceData {
        Napi::FunctionReference* connectionConstructor = nullptr;
        Napi::FunctionReference* clientDataAreaConstructor = nullptr;
    };
}
}
