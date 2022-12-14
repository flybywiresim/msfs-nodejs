const { Connection, ClientDataArea } = require('../../src/bindings/simconnect/build/Release/simconnect');

test('ClientDataArea', () => {
    const connection = new Connection();
    const area = new ClientDataArea(connection);
});
