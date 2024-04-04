![FlyByWire Simulations](https://raw.githubusercontent.com/flybywiresim/branding/master/tails-with-text/FBW-Color-Dark.svg#gh-light-mode-only)
![FlyByWire Simulations](https://raw.githubusercontent.com/flybywiresim/branding/master/tails-with-text/FBW-Color-Light.svg#gh-dark-mode-only)

# FlyByWire Simulations API Client

The Node.JS wrapper around the MSFS SDK.
The library supports both JavaScript and TypeScript.

## Installation

Install the client library using npm:

    $ npm install --save @flybywiresim/msfs-nodejs

## Build steps

Install [node-gyp](https://github.com/nodejs/node-gyp) on the system and follow the the installation instructions.

Start the build process by `npm run build:all`.

### Hotfixes for build issues

If the binding-compilation fails due to v8.h include errors is it required to delete the local cache of node-gyp.
node-gyp cache: `%LocalAppData%/node-gyp/Cache`
Thereafter execute `node-gyp configure` and the problem should be solved.

## Release steps

Some manual steps are required to release the package.
These steps are required to avoid the creation a Windows-VM on Github.

- Build the bindings with Visual Studio on your local machine
- Call `npm run build:rollup`
- Commit and push the new generated *.node-bindings
- Create a new tag on github and publish the release

## License

This software is licensed under the [MIT license](https://github.com/flybywiresim/api-client/blob/main/LICENSE).
