const { Connection } = require('../../src/bindings/simconnect/build/Release/simconnect');

test('IsConnected - New instance', () => {
    const connection = new Connection();
    expect(connection.isConnected()).toBe(false);
});

test('IsConnected - Wrong parameter count', () => {
    const connection = new Connection();
    expect(() => connection.isConnected(12)).toThrow('Parameters are not allowed');
});

test('LastError - New instance', () => {
    const connection = new Connection();
    expect(connection.lastError()).toBe('');
});

test('LastError - Wrong parameter count', () => {
    const connection = new Connection();
    expect(() => connection.lastError(12)).toThrow('Parameters are not allowed');
});

test('Open - No parameters', () => {
    const connection = new Connection();
    expect(() => connection.open()).toThrow('Wrong number of arguments');
});

test('Open - Too many parameters', () => {
    const connection = new Connection();
    expect(() => connection.open('test', 'test')).toThrow('Wrong number of arguments');
});

test('Open - Invalid parameter type - Number', () => {
    const connection = new Connection();
    expect(() => connection.open(12)).toThrow("Invalid argument type. 'name' must be a string");
});

test('Open - Invalid parameter type - Null', () => {
    const connection = new Connection();
    expect(() => connection.open(null)).toThrow("Invalid argument type. 'name' must be a string");
});

test('Open - Invalid parameter type - Object', () => {
    const connection = new Connection();
    expect(() => connection.open({ test: 'test' })).toThrow("Invalid argument type. 'name' must be a string");
});

test('Open - Shut down server', () => {
    const connection = new Connection();
    expect(connection.open('test')).toBeFalsy();
    expect(connection.lastError()).toBe('Unable to open a connection: Unknown exception');
});

test('NewClientDataArea - not connected', () => {
    const connection = new Connection();
    expect(() => connection.newClientDataArea(1000)).toThrow('Not connected to the server');
});

test('AddClientDataDefinition - not connected', () => {
    const connection = new Connection();
    expect(() => connection.addClientDataDefinition({
        offset: 0,
        sizeOrType: 10,
        epsilon: 0,
        memberName: 'test',
    })).toThrow('Not connected to the server');
});

test('ClearClientDataDefinition - not connected', () => {
    const connection = new Connection();
    expect(() => connection.clearClientDataDefinition('test')).toThrow('Not connected to the server');
});
