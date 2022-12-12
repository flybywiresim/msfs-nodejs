import { ClientDataArea } from './clientdataarea';
import { ClientDataDefinition } from './clientdatadefinition';

const { Wrapper } = require('./libs/simconnect');

export class ClientData {
    private wrapper: typeof Wrapper = null;

    constructor(wrapper: typeof Wrapper) {
        this.wrapper = wrapper;
    }

    public addDataDefinition(definition: ClientDataDefinition): boolean {
        return this.wrapper.addClientDataDefinition(definition);
    }

    public createDataArea(clientDataId: number, name: string, size: number, readOnly: boolean): ClientDataArea {
        if (this.wrapper.newClientDataArea(clientDataId)) {
            const instance = new ClientDataArea(this, clientDataId);
            if (!instance.mapNameToId(name)) return null;
            if (!instance.createDataArea(size, readOnly)) return null;
            return instance;
        }

        return null;
    }

    public errorMessage(): string {
        return this.wrapper.errorMessage();
    }
}
