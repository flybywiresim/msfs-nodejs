import { Connection, ErrorMessage, ExceptionMessage, OpenMessage, Receiver, SimulatorDataArea, SimulatorDataPeriod, SimulatorDataRequestMessage, SimulatorDataType } from '../../src';

const enum SimulatorDataId {
    TestData = 123,
}

let connection: Connection | null = null;
let receiver: Receiver | null = null;
let testData: SimulatorDataArea | null = null;

const simConnectOpen = (message: OpenMessage): void => {
    console.log(`Connected to ${message.application.name} - v${message.application.version.major}.${message.application.version.minor}`);

    if (testData !== null && receiver !== null) {
        if (receiver.requestSimulatorData(testData, SimulatorDataPeriod.Once)) {
            console.log('Unable to request the simulator data');
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

const simConnectSimulatorDataRequest = (message: SimulatorDataRequestMessage): void => {
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
    if (!connection.open('TEST_RECEIVER')) {
        console.log(`Connection failed: ${connection.lastError()} - Retry in 10 seconds`);
        testData = null;
        receiver = null;
        connection = null;
        setTimeout(() => connectToSim(), 10000);
        return;
    }

    createTestArea();

    receiver = new Receiver(connection);
    receiver.addCallback('open', (message: OpenMessage) => simConnectOpen(message));
    receiver.addCallback('quit', () => simConnectQuit());
    receiver.addCallback('simulatorData', (message: SimulatorDataRequestMessage) => simConnectSimulatorDataRequest(message));
    receiver.addCallback('exception', (message: ExceptionMessage) => simConnectException(message));
    receiver.addCallback('error', (message: ErrorMessage) => simConnectError(message));
    receiver.start();
};

const createTestArea = () => {
    if (connection === null) return;

    testData = new SimulatorDataArea(connection, SimulatorDataId.TestData);

    const addedDefinition = testData.addDataDefinition({
        name: 'INDICATED ALTITUDE',
        memberName: 'indicatedAltitude',
        type: SimulatorDataType.Float64,
        unit: 'feet',
    });
    if (!addedDefinition) {
        console.log('Unable to add definition');
    }
};

connectToSim();
setInterval(() => {}, 2000);
