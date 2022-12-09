#pragma once

#include <string>
#include <SimConnect.h>

namespace msfs {
namespace nodejs {
namespace simconnect {
    class Helper {
        public:
            static std::string translateException(SIMCONNECT_EXCEPTION exception);
    };
}
}
}
