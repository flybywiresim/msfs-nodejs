#pragma once

#include <Windows.h>
#include <napi.h>

namespace msfs {
namespace simconnect {
    class Connection : public Napi::ObjectWrap<Connection> {
    private:
        HANDLE _simConnect;
        std::string _lastError;

        void close();
    public:
        /**
         * @brief Constructs a new Connection object
         * @param info Parameter block for the Connection
         */
        Connection(const Napi::CallbackInfo& info);
        /**
         * @brief Destroys the Connection object
         */
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
         * @brief Returns the last error of an other call
         * @param info The parameter block without additional parameters
         * @return Returns Napi::String with the last error
         */
        Napi::Value lastError(const Napi::CallbackInfo& info);

        /**
         * @brief Creates a new Connection object
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