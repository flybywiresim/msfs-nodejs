export interface DispatcherResponse {
    type: 'open' | 'quit' | 'clientData' | 'simulatorData' | 'systemEvent' | 'exception' | 'error';
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

export * from './clientdata';
export * from './simulatordata';
export * from './systemevent';
