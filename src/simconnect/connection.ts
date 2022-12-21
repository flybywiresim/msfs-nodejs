const simconnect = require('./libs/simconnect');

export interface Connection {
    open(clientName: string): boolean;
    close(): void;
    isConnected(): boolean;
    lastError(): string;
}

export const Connection: {
    new(): Connection
} = simconnect.ConnectionBindings;
