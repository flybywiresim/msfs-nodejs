{
  "targets": [
    {
      "target_name": "simconnect",
      "sources": [
        "clientdata.cc",
        "connection.cc",
        "dispatch.cc",
        "helper.cc",
        "simconnect.cc",
        "wrapper.cc"
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "include_dirs": [
        "C:\\MSFS SDK\\SimConnect SDK\\include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "libraries": [
        "C:\\MSFS SDK\\SimConnect SDK\\lib\\SimConnect.lib"
      ]
    }
  ],
}
