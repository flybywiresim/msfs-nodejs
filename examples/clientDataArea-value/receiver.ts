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
    ClientDataType,
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
    const data = entry[1] as Number;
    console.log(`Received data: ${data}`);
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
        definitionId: DataDefinitionId.Float64Value,
        offset: ClientDataOffsetAuto,
        sizeOrType: ClientDataType.Float64,
        epsilon: 0.0,
        memberName: 'TestData',
    });
    if (!addedDefinition) {
        console.log('Unable to add the definition');
    }
};

connectToSim();
setInterval(() => {}, 2000);
