{
  "targets": [
    {
      "target_name": "simconnect",
      "sources": [
        "clientdataarea.cc",
        "connection.cc",
        "dispatcher.cc",
        "helper.cc",
        "simconnect.cc",
        "simulatordataarea.cc",
        "systemevent.cc"
      ],
      "msvs_settings": {
        'VCCLCompilerTool': {
          'AdditionalOptions': [
            '/MD'
          ],
        }
      },
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "include_dirs": [
        "C:\\MSFS SDK\\SimConnect SDK\\include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "libraries": [
        "C:\\MSFS SDK\\SimConnect SDK\\lib\\static\\SimConnect.lib",
      ]
    }
  ],
}
