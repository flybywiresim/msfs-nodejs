import {
    Connection,
    ClientDataType,
    Receiver,
    OpenMessage,
    ExceptionMessage,
    ErrorMessage,
    ClientDataArea,
    ClientDataOffsetAuto,
} from '../../src';

const enum ClientDataId {
    TestData = 0,
}

const enum DataDefinitionId {
    Float64Value = 0,
}

let connection: Connection | null = null;
let receiver: Receiver | null = null;
let thresholdData: ClientDataArea | null = null;
let data = 2.5;

const simConnectOpen = (message: OpenMessage): void => {
    console.log(`Connected to ${message.application.name} - v${message.application.version.major}.${message.application.version.minor}`);
};

const simConnectQuit = (): void => {
    console.log('quit');
};

const simConnectError = (message: ErrorMessage): void => {
    if (message !== null) {
        console.log(`Error: ${message.id}`);
    } else {
        console.log('Invalid error');
    }
};

const simConnectException = (message: ExceptionMessage): void => {
    console.log(`Exception: ${message.exception} ${message.sendId} ${message.index}`);
    console.log(message.exceptionText);
};

const connectToSim = () => {
    connection = new Connection();
    if (connection.open('TEST_SENDER') === false) {
        console.log(`Connection failed: ${connection.lastError()} - Retry in 10 seconds`);
        thresholdData = null;
        receiver = null;
        connection = null;
        setTimeout(() => connectToSim(), 10000);
        return;
    }

    createThresholdArea();

    if (receiver !== null) receiver.stop();
    receiver = new Receiver(connection);
    receiver.addCallback('open', (message: OpenMessage) => simConnectOpen(message));
    receiver.addCallback('quit', () => simConnectQuit());
    receiver.addCallback('exception', (message: ExceptionMessage) => simConnectException(message));
    receiver.addCallback('error', (message: ErrorMessage) => simConnectError(message));
    receiver.start();
};

const createThresholdArea = () => {
    if (connection === null) return;

    thresholdData = new ClientDataArea(connection, ClientDataId.TestData);
    if (!thresholdData.mapNameToId('TEST_DATA')) {
        console.log('Unable to map the ID');
    }

    const addedDefinition = thresholdData.addDataDefinition({
        definitionId: DataDefinitionId.Float64Value,
        offset: ClientDataOffsetAuto,
        sizeOrType: ClientDataType.Float64,
        epsilon: 0.0,
        memberName: 'TestData',
    });
    if (!addedDefinition) {
        console.log('Unable to add the definitions');
    }

    thresholdData.allocateArea(8, true);
};

connectToSim();
setInterval(() => {
    if (connection !== null && connection.isConnected()) {
        if (thresholdData?.setData({ TestData: data }) === false) {
            console.log(`Error: ${thresholdData.lastError()}`);
        } else {
            console.log('Sent data');
            data += 1.0;
        }
    }
}, 2000);
