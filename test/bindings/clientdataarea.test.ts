const { ConnectionBindings, ClientDataAreaBindings } = require('../../src/bindings/simconnect/build/Release/simconnect');

test('Constructor - Not enough arguments', () => {
    const connection = new ConnectionBindings();
    expect(() => new ClientDataAreaBindings()).toThrow('Wrong number of arguments');
    expect(() => new ClientDataAreaBindings(connection)).toThrow('Wrong number of arguments');
});

test('Constructor - Too many arguments', () => {
    const connection = new ConnectionBindings();
    expect(() => new ClientDataAreaBindings(connection, 1000, 5)).toThrow('Wrong number of arguments');
});

test('Constructor - Valid input', () => {
    const connection = new ConnectionBindings();
    expect(new ClientDataAreaBindings(connection, 1000)).toBeInstanceOf(ClientDataAreaBindings);
});

test('Constructor - Used default client data IDs', () => {
    const connection = new ConnectionBindings();
    expect(() => new ClientDataAreaBindings(connection, 0)).toThrow('The client data ID already exists');
});

test('Constructor - Used client data IDs', () => {
    const connection = new ConnectionBindings();
    expect(new ClientDataAreaBindings(connection, 1000)).toBeInstanceOf(ClientDataAreaBindings);
    expect(() => new ClientDataAreaBindings(connection, 1000)).toThrow('The client data ID already exists');
});

test('MapNameToId - Not connected', () => {
    const connection = new ConnectionBindings();
    const area = new ClientDataAreaBindings(connection, 1000);
    expect(() => area.mapNameToId('test')).toThrow('Not connected to the server');
});

test('AllocateArea - Not connected', () => {
    const connection = new ConnectionBindings();
    const area = new ClientDataAreaBindings(connection, 1000);
    expect(() => area.allocateArea(10, false)).toThrow('Not connected to the server');
});

test('SetData - Not connected', () => {
    const connection = new ConnectionBindings();
    const area = new ClientDataAreaBindings(connection, 1000);
    expect(() => area.setData({ test: 5 })).toThrow('Not connected to the server');
});
