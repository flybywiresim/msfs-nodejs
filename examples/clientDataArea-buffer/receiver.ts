import {
    Connection,
    Receiver,
    OpenMessage,
    ExceptionMessage,
    ErrorMessage,
    ClientDataArea,
    ClientDataOffsetAuto,
    ClientDataRequestMessage,
    ClientDataPeriod,
    ClientDataRequest,
} from '../../src';

const enum ClientDataId {
    TestData = 0,
}

const enum DataDefinitionId {
    DataBuffer = 0,
}

interface ReceivedData {
    Field0: number;
    Field1: number;
    Field2: number;
    Field3: number;
    Field4: number;
    Field5: number;
}

let connection: Connection | null = null;
let receiver: Receiver | null = null;
let thresholdData: ClientDataArea | null = null;

const simConnectOpen = (message: OpenMessage): void => {
    console.log(`Connected to ${message.application.name} - v${message.application.version.major}.${message.application.version.minor}`);

    if (thresholdData !== null && receiver !== null) {
        if (receiver.requestClientData(thresholdData, ClientDataPeriod.OnSet, ClientDataRequest.Default) === false) {
            console.log('Unable to request the client data');
        }
    }
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

const simConnectClientDataRequest = (message: ClientDataRequestMessage): void => {
    const entry = Object.entries(message.content)[0];
    const data = entry[1] as ArrayBuffer;

    const buffer = Buffer.from(data);

    const receivedData: ReceivedData = {
        Field0: 0,
        Field1: 0,
        Field2: 0,
        Field3: 0,
        Field4: 0,
        Field5: 0,
    };
    receivedData.Field0 = buffer.readInt16LE(0);
    receivedData.Field1 = buffer.readInt16LE(2);
    receivedData.Field2 = buffer.readInt16LE(4);
    receivedData.Field3 = buffer.readUint8(6);
    receivedData.Field4 = buffer.readInt16LE(7);
    receivedData.Field5 = buffer.readUint8(9);

    console.log(receivedData);
};

const simConnectException = (message: ExceptionMessage): void => {
    console.log(`Exception: ${message.exception} ${message.sendId} ${message.index}`);
    console.log(message.exceptionText);
};

const connectToSim = () => {
    connection = new Connection();
    if (connection.open('TEST_RECEIVER') === false) {
        console.log(`Connection failed: ${connection.lastError()} - Retry in 10 seconds`);
        thresholdData = null;
        receiver = null;
        connection = null;
        setTimeout(() => connectToSim(), 10000);
        return;
    }

    createThresholdArea();

    receiver = new Receiver(connection);
    receiver.addCallback('open', (message: OpenMessage) => simConnectOpen(message));
    receiver.addCallback('quit', () => simConnectQuit());
    receiver.addCallback('clientData', (message: ClientDataRequestMessage) => simConnectClientDataRequest(message));
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
        definitionId: DataDefinitionId.DataBuffer,
        offset: ClientDataOffsetAuto,
        sizeOrType: 10,
        epsilon: 0.0,
        memberName: 'TestData',
    });
    if (!addedDefinition) {
        console.log('Unable to add the definition');
    }
};

connectToSim();
setInterval(() => {}, 2000);
