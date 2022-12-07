# <img src="https://raw.githubusercontent.com/flybywiresim/fbw-branding/master/svg/FBW-Logo.svg" placeholder="FlyByWire" width="400"/>
# FlyByWire Simulations API Client

The Node.JS wrapper around the MSFS SDK.
The library supports both JavaScript and TypeScript.

## Installation

Install the client library using npm:

    $ npm install --save @flybywiresim/msfs-nodejs

## Build steps

Install [node-gyp](https://github.com/nodejs/node-gyp) on the system and follow the the installation instructions.

Start the build process by `npm run build`.

### Hotfixes for build issues

If the binding-compilation fails due to v8.h include errors is it required to delete the local cache of node-gyp.
node-gyp cache: `%LocalAppData%/node-gyp/Cache`
Thereafter execute `node-gyp configure` and the problem should be solved.

## License

This software is licensed under the [MIT license](https://github.com/flybywiresim/api-client/blob/main/LICENSE).
