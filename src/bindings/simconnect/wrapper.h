#pragma once

#include <Windows.h>
#include <SimConnect.h>
#include <napi.h>

#include <list>
#include <map>

namespace msfs {
namespace simconnect {
    class Wrapper : public Napi::ObjectWrap<Wrapper> {
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
        template <typename T>
        bool setClientDataNumber(SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                 SIMCONNECT_CLIENT_DATA_DEFINITION_ID definitionId,
                                 const Napi::Value& value);
        bool setClientDataField(SIMCONNECT_CLIENT_DATA_ID clientDataId,
                                const ClientDataDefinition& definition,
                                const Napi::Object& object);

        void close();
    public:
        /**
         * @brief Constructs a new Wrapper object
         * @param info Parameter block for the Wrapper
         */
        Wrapper(const Napi::CallbackInfo& info);
        /**
         * @brief Destroys the Wrapper object
         */
        ~Wrapper();

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
         * @brief Creates a new client data area without registering it on the server
         * @param info The parameters with the client data ID
         * @return True, if the new ID is created, else false with the last error set
         */
        Napi::Value newClientDataArea(const Napi::CallbackInfo& info);
        /**
         * @brief Maps the client data name to the ID on the server
         * @param info The info block with the parameters for the clientDataName and the clientDataid
         * @return True if the mapping was successful, else false with the last error set
         */
        Napi::Value mapClientDataNameToId(const Napi::CallbackInfo& info);
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
         * @brief Sets the client data entries
         * @param info The info block with the parameters clientDataId and the partial or full content block with properties as the data definition
         * @return True if all entries in the data definition are set, else False
         */
        Napi::Value setClientData(const Napi::CallbackInfo& info);
        void nextDispatch(const Napi::CallbackInfo& info);
        /**
         * @brief Returns the last error of an other call
         * @param info The parameter block without additional parameters
         * @return Returns Napi::String with the last error
         */
        Napi::Value lastError(const Napi::CallbackInfo& info);

        /**
         * @brief Creates a new Wrapper object
         * @param info Parameter block for the new object
         * @return Returns the newly created object
         */
        static Napi::Value createNewItem(const Napi::CallbackInfo& info);
        /**
         * @brief Registers all binding functions in the JS environment
         * @param env The javascript environment
         * @param exports The collection of exported functions
         * @return The extended exported functions
         */
        static Napi::Object initialize(Napi::Env env, Napi::Object exports);
    };
}
}
