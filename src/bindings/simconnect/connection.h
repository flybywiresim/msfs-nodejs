#pragma once

#include <string>
#include <list>

#include <napi.h>
#include <Windows.h>
#include <SimConnect.h>

namespace msfs {
namespace simconnect {
    class ClientDataArea;
    class Dispatcher;

    class Connection : public Napi::ObjectWrap<Connection> {
        friend ClientDataArea;
        friend Dispatcher;

    private:
        struct ClientDataDefinition {
            SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId;
            DWORD offset;
            DWORD sizeOrType;
            float epsilon;
            std::string memberName;
        };

        HANDLE _simConnect;
        std::string _lastError;
        std::list<SIMCONNECT_CLIENT_DATA_ID> _clientDataIds;
        SIMCONNECT_CLIENT_DATA_DEFINITION_ID _clientDataDefinitionIdCounter;
        std::list<ClientDataDefinition> _clientDataDefinitions;

        bool clientDataIdExists(SIMCONNECT_CLIENT_DATA_ID clientDataId) const;
        void close();

    public:
        Connection(const Napi::CallbackInfo& info);
        ~Connection();

        /**
         * @brief Opens a SimConnect connection to the server
         * @param info The callback block where the first element needs to be the client's name
         * @return Returns a Napi::Boolean and sets the last error, if the function returned false
         * @throw Excpetions if the arguments do not match
         */
        Napi::Value open(const Napi::CallbackInfo& info);
        /**
         * @brief Closes a SimConnect connection
         * @param info The parameter block without additional parameters
         */
        void close(const Napi::CallbackInfo& info);
        /**
         * @brief Checks if the SimConnect connection is actove
         * @param info The parameter block without additional parameters
         * @return True if the connection is active, else false
         */
        Napi::Value isConnected(const Napi::CallbackInfo& info);
        /**
         * @brief Adds a new client data definition
         * @param info The info block with parameter the data definition
         * @return True if the definition is added, else false
         */
        Napi::Value addClientDataDefinition(const Napi::CallbackInfo& info);
        /**
         * @brief Clears a client data definition
         * @param info The info block with the parameter for data definition name
         * @return True if the definition is cleared, else false
         */
        Napi::Value clearClientDataDefinition(const Napi::CallbackInfo& info);
        /**
         * @brief Creates anew client data area on the server
         * @param info The info block with the paramaters clientDataId, size and readOnly
         * @return True if the creation was successful, else false with the last error set
         */
        Napi::Value createClientDataArea(const Napi::CallbackInfo& info);
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
