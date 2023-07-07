#pragma once

#include <napi.h>

#include "connection.h"

namespace msfs {
namespace simconnect {
    class SystemEvent : public Napi::ObjectWrap<SystemEvent> {
    private:
        Connection* _connection;
        std::string _event;
        std::uint32_t _id;
        std::string _lastError;

    public:
        SystemEvent(const Napi::CallbackInfo& info);

        /**
         * @brief Returns the unique event ID
         * @return Returns the unique event ID as std::uint32_t
         */
        std::uint32_t id() const;
        /**
         * @brief Returns the event name
         * @return Returns the event name as std::string
         */
        const std::string& eventName() const;
        /**
         * @brief Subscribes the event to the SimConnect connection
         * @return Return true if the event is subscribed, else false
         */
        bool subscribe();
        /**
         * @brief Unubscribes the event from the SimConnect connection
         * @return Return true if the event is unsubscribed, else false
         */
        bool unsubscribe();
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
