#pragma once

#include <napi.h>

#include "connection.h"

namespace msfs {
namespace simconnect {
    class SimulatorDataArea : public Napi::ObjectWrap<SimulatorDataArea> {
    public:
        struct SimulatorDataDefinition {
            SIMCONNECT_DATATYPE type;
            std::string name;
            std::string unit;
            std::string memberName;
        };

    private:
        Connection* _connection;
        SIMCONNECT_DATA_DEFINITION_ID _id;
        SIMCONNECT_DATA_REQUEST_ID _requestId;
        std::list<SimulatorDataDefinition> _dataDefinitions;
        std::string _lastError;

    public:
        SimulatorDataArea(const Napi::CallbackInfo& info);

        /**
         * @brief Returns the simulator data definition ID
         * @return The simulator data definition ID
         */
        const SIMCONNECT_DATA_DEFINITION_ID& id() const;
        /**
         * @brief Adds a new client data definition
         * @param info The info block with parameter the data definition
         * @return True if the definition is added, else false
         */
        Napi::Value addDataDefinition(const Napi::CallbackInfo& info);
        /**
         * @brief Requests the data of the client data area
         * @param[in,out] requestId The request ID and will be updated with every call
         * @param[in] period The period to request the data
         * @return True if all fields are requested, else false
         */
        bool requestData(SIMCONNECT_DATA_REQUEST_ID requestId, SIMCONNECT_PERIOD period);
        /**
         * @brief Requests the data of the simulator data area with already existing request IDs
         * @param[in] period The period to request the data
         * @return True if all fields are requested, else false
         */
        bool updateRequestData(SIMCONNECT_PERIOD period);
        /**
         * @brief Returns the list of registered data elements
         * @return The list of all data definitions
         */
        const std::list<SimulatorDataDefinition>& dataDefinitions() const;
        /**
         * @brief Returns the last error of an other call
         * @param info The parameter block without additional parameters
         * @return Returns Napi::String with the last error
         */
        Napi::Value lastError(const Napi::CallbackInfo& info);

        static Napi::Object initialize(Napi::Env env, Napi::Object exports);
    };
}
}
