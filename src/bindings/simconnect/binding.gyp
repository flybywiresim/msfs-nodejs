{
  "targets": [
    {
      "target_name": "simconnect",
      "sources": [
        "clientdata.cc",
        "connection.cc",
        "helper.cc",
        "simconnect.cc",
        "wrapper.cc"
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
