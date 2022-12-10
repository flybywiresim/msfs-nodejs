{
  "targets": [
    {
      "target_name": "msfs",
      "sources": [
        "src/bindings/simconnect/wrapper.cc",
        "src/bindings/simconnect/helper.cc",
        "src/bindings/msfs.cc"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "include_dirs": [
        "C:\\MSFS SDK\\SimConnect SDK\\include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "libraries": [
        "C:\\MSFS SDK\\SimConnect SDK\\lib\\SimConnect.lib"
      ]
    }
  ],
}
