#include "helper.h"
#include "wrapper.h"

#include <SimConnect.h>

using namespace msfs::simconnect;

void Wrapper::close() {
    if (this->_simConnect != 0) {
        SimConnect_Close(this->_simConnect);
        this->_simConnect = 0;
    }
}

Napi::Value Wrapper::open(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Invalid argument type. 'name' must be a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (this->_simConnect == 0) {
        Napi::String name = info[0].As<Napi::String>();
        std::string nameStr = name.Utf8Value();
        HRESULT result = SimConnect_Open(&this->_simConnect, nameStr.c_str(), nullptr, 0, 0, 0);
        if (result != S_OK) {
            this->_lastError = "Unable to open a Wrapper: " + Helper::translateException((SIMCONNECT_EXCEPTION)result);
            return Napi::Boolean::New(env, false);
        }
    }

    return Napi::Boolean::New(env, true);
}

void Wrapper::close(const Napi::CallbackInfo& info) {
    std::ignore = info;
    this->close();
}
