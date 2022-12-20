import { SimulatorDataType } from './constants';

export interface ClientDataDefinition {
    definitionId: number;
    offset: number;
    sizeOrType: number;
    epsilon: number;
    memberName: string;
}

export interface SimulatorDataDefinition {
    type: SimulatorDataType,
    name: string,
    unit: string,
    memberName: string,
}

export interface DispatcherResponse {
    type: 'open' | 'quit' | 'clientData' | 'simulatorData' | 'exception' | 'error';
    data: object;
}

export interface Version {
    major: number;
    minor: number;
}

export interface OpenMessage {
    application: {
        name: string;
        version: Version;
        build: Version;
    }
    simconnect: {
        version: Version;
        build: Version;
    }
}

export interface ExceptionMessage {
    exception: number;
    exceptionText: string;
    sendId: number;
    index: number;
}

export interface ErrorMessage {
    id: number;
    size: number;
    version: number;
}

export interface ClientDataRequestMessage {
    clientDataId: number;
    content: object;
}

export interface SimulatorDataRequestMessage {
    definitionId: number;
    content: object;
}
