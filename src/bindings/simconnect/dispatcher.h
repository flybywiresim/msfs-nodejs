#pragma once

#include <napi.h>

#include "clientdataarea.h"
#include "connection.h"
#include "simulatordataarea.h"

namespace msfs {
namespace simconnect {
    class Dispatcher : public Napi::ObjectWrap<Dispatcher> {
    private:
        Connection* _connection;
        SIMCONNECT_DATA_REQUEST_ID _requestId;
        std::list<ClientDataArea*> _requestedClientAreas;
        std::list<SimulatorDataArea*> _requestedSimulatorDataArea;
        std::string _lastError;

        static Napi::Object convertOpenMessage(Napi::Env env, SIMCONNECT_RECV_OPEN* message);
        static Napi::Object convertUnknownMessage(Napi::Env env, SIMCONNECT_RECV* message);
        static Napi::Object convertExceptionMessage(Napi::Env env, SIMCONNECT_RECV_EXCEPTION* message);
        static Napi::Object convertClientDataAreaMessage(Napi::Env env, SIMCONNECT_RECV_CLIENT_DATA* message,
                                                         SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                                         const ClientDataArea::ClientDataDefinition& definition);

    public:
        Dispatcher(const Napi::CallbackInfo& info);

        /**
         * @brief Requests all elements of a ClientDataArea
         * @param info The information block with the ClientDataArea, the period and the flag
         * @return True if the entries are requested, else false
         */
        Napi::Value requestClientData(const Napi::CallbackInfo& info);
        /**
         * @brief Requests all elements of a ClientDataArea
         * @param info The information block with the SimulatorDataArea, the period and the flag
         * @return True if the entries are requested, else false
         */
        Napi::Value requestSimulatorData(const Napi::CallbackInfo& info);
        /**
         * @brief Processes the next dispatch and triggers the corresponding events
         * @param info The information block without additional parameters
         * @return True if a dispatch call was executed, else false
         */
        Napi::Value nextDispatch(const Napi::CallbackInfo& info);
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
