#pragma once

#include <Windows.h>
#include <SimConnect.h>
#include <napi.h>

#include <list>
#include <map>

namespace msfs {
namespace simconnect {
    class Wrapper : public Napi::ObjectWrap<Wrapper> {
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
