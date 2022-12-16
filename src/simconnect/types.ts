export interface ClientDataDefinition {
    offset: number;
    sizeOrType: number;
    epsilon: number;
    memberName: string;
}

export interface Version {
    major: number;
    minor: number;
}

export interface ErrorMessage {
    id: number;
    size: number;
    version: number;
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
