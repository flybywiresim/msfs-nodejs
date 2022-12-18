#pragma once

#include <napi.h>

#include "connection.h"

namespace msfs {
namespace simconnect {
    class ClientDataArea : public Napi::ObjectWrap<ClientDataArea> {
    private:
        struct ClientDataDefinition {
            SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId;
            DWORD offset;
            DWORD sizeOrType;
            float epsilon;
            std::string memberName;
        };

        Connection* _connection;
        SIMCONNECT_CLIENT_DATA_ID _id;
        std::list<ClientDataDefinition> _clientDataDefinitions;
        std::string _lastError;

        template <typename T>
        bool setClientDataNumber(SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId,
                                 const Napi::Value& value);
        bool setClientDataField(const ClientDataDefinition& definition,
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
         * @brief Adds a new client data definition
         * @param info The info block with parameter the data definition
         * @return True if the definition is added, else false
         */
        Napi::Value addDataDefinition(const Napi::CallbackInfo& info);
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
        /**
         * @brief Requests the data of the client data area
         * @param info The info block with the parameters for the request IDs, the period flag and the flag
         * @return True if the data is requested, else false
         */
        Napi::Value requestData(const Napi::CallbackInfo& info);
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
