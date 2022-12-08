import { ClientDataDefinition } from './clientdatadefinition';

const msfs = require('./libs/msfs');

export class ClientData {
    private constructor(name: string, dataId: number, datasize: number, readOnly: boolean) {
        msfs.mapClientDataNameToId(name, dataId);
        msfs.createClientData(dataId, datasize, readOnly);
    }

    public static create(name: string, dataId: number, datasize: number, readOnly: boolean): Promise<ClientData> {
        return new Promise<ClientData>((resolve, reject) => {
            try {
                const data = new ClientData(name, dataId, datasize, readOnly);
                resolve(data);
            } catch (err) {
                reject(err);
            }
            return null;
        });
    }

    public addDataDefinition(definition: ClientDataDefinition) {
        msfs.addToClientDataDefinition(definition.dataDefinitionId, definition.offset, definition.sizeOrType, definition.epsilon);
    }
}
