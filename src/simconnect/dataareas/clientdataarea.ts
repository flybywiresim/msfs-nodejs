import { Connection } from '../connection';
import { ClientDataDefinition } from '../types';

const simconnect = require('./libs/simconnect');

export interface ClientDataArea {
    mapNameToId(clientDataName: string): boolean;
    allocateArea(size: number, readOnly: boolean): boolean;
    addDataDefinition(definition: ClientDataDefinition): boolean;
    setData(data: object): boolean;
    lastError(): string;
}

export const ClientDataArea: {
    new(connection: Connection, clientDataId: number): ClientDataArea,
} = simconnect.ClientDataAreaBindings;
