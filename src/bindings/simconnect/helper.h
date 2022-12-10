#pragma once

#include <string>
#include <Windows.h>
#include <SimConnect.h>

namespace msfs {
namespace simconnect {
    class Helper {
        public:
            static std::string translateException(SIMCONNECT_EXCEPTION exception);
    };
}
}
