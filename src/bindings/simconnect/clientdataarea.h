#include <napi.h>

#include "connection.h"

namespace msfs {
namespace simconnect {
    class ClientDataArea : public Napi::ObjectWrap<ClientDataArea> {
    private:
        Connection* _connection;
        std::string _lastError;

        static Napi::FunctionReference constructor;

        template <typename T>
        bool setClientDataNumber(SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                 SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId,
                                 const Napi::Value& value);
        bool setClientDataField(SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                const Connection::ClientDataDefinition& definition,
                                const Napi::Object& object);

    public:
        ClientDataArea(const Napi::CallbackInfo& info);

        /**
         * @brief Maps the client data name to the ID on the server
         * @param info The info block with the parameters for the clientDataName and the clientDataid
         * @return True if the mapping was successful, else false with the last error set
         */
        Napi::Value mapClientDataNameToId(const Napi::CallbackInfo& info);
        /**
         * @brief Creates anew client data area on the server
         * @param info The info block with the paramaters clientDataId, size and readOnly
         * @return True if the creation was successful, else false with the last error set
         */
        Napi::Value createClientDataArea(const Napi::CallbackInfo& info);
        /**
         * @brief Sets the client data entries
         * @param info The info block with the parameters clientDataId and the partial or full content block with properties as the data definition
         * @return True if all entries in the data definition are set, else False
         */
        Napi::Value setClientData(const Napi::CallbackInfo& info);

        static Napi::Object initialize(Napi::Env env, Napi::Object exports);
    };
}
}
