#pragma once

#include <napi.h>

#include "connection.h"

namespace msfs {
namespace simconnect {
    class ClientDataArea : public Napi::ObjectWrap<ClientDataArea> {
    private:
        Connection* _connection;
        SIMCONNECT_CLIENT_DATA_ID _id;
        std::string _lastError;

        template <typename T>
        bool setClientDataNumber(SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId,
                                 const Napi::Value& value);
        bool setClientDataField(const Connection::ClientDataDefinition& definition,
                                const Napi::Object& object);

    public:
        ClientDataArea(const Napi::CallbackInfo& info);

        /**
         * @brief Maps the client data name to the ID on the server
         * @param info The info block with the parameter for the clientDataName
         * @return True if the mapping was successful, else false with the last error set
         */
        Napi::Value mapNameToId(const Napi::CallbackInfo& info);
        /**
         * @brief Creates a new client data area on the server
         * @param info The info block with the parameters size and readOnly
         * @return True if the creation was successful, else false with the last error set
         */
        Napi::Value allocateArea(const Napi::CallbackInfo& info);
        /**
         * @brief Sets the client data entries
         * @param info The info block with the parameter of the partial or full content block with properties as the data definition
         * @return True if all entries in the data definition are set, else False
         */
        Napi::Value setData(const Napi::CallbackInfo& info);

        static Napi::Object initialize(Napi::Env env, Napi::Object exports);
    };
}
}
