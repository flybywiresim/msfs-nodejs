import { Connection } from '../connection';
import { ClientDataDefinition } from '../types';
export interface ClientDataArea {
    mapNameToId(clientDataName: string): boolean;
    allocateArea(size: number, readOnly: boolean): boolean;
    addDataDefinition(definition: ClientDataDefinition): boolean;
    setData(data: object): boolean;
    lastError(): string;
}
export declare const ClientDataArea: {
    new (connection: Connection, clientDataId: number): ClientDataArea;
};
