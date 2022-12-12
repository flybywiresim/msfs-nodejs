import { ClientDataDefinition } from './clientdatadefinition';

const { Wrapper } = require('./libs/simconnect');

export class ClientDataArea {
    private wrapper: typeof Wrapper = null;

    private clientDataId: number;

    constructor(wrapper: typeof Wrapper, clientDataId: number) {
        this.wrapper = wrapper;
        this.clientDataId = clientDataId;
    }

    public mapNameToId(name: string): boolean {
        return this.wrapper.mapClientDataNameToId(this.clientDataId, name);
    }

    public addDataDefinition(definition: ClientDataDefinition): boolean {
        return this.wrapper.addClientDataDefinition(definition);
    }

    public createDataArea(size: number, readOnly: boolean): boolean {
        return this.wrapper.createClientDataArea(this.clientDataId, size, readOnly);
    }

    public setData(data: object): boolean {
        return this.wrapper.setClientData(this.clientDataId, data);
    }

    public errorMessage(): string {
        return this.wrapper.lastError();
    }
}
