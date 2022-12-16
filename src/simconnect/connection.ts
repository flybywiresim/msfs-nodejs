import { ClientDataDefinition } from './types';

const simconnect = require('./libs/simconnect');

export interface Connection {
    open(clientName: string): boolean;
    close(): void;
    isConnected(): boolean;
    addClientDataDefinition(definition: ClientDataDefinition): boolean;
    clearClientDataDefinition(memberName: string): boolean;
    lastError(): string;
}

export const Connection: {
    new(): Connection
} = simconnect.ConnectionBindings;
