import { ClientData } from './clientdata';

const { Wrapper } = require('./libs/simconnect');

export class Connection {
    private wrapper: typeof Wrapper = null;

    private clientDataManagement: ClientData = null;

    constructor() {
        this.wrapper = new Wrapper();
        this.clientDataManagement = new ClientData(this.wrapper);
    }

    public open(clientName: string): boolean {
        return this.wrapper.open(clientName);
    }

    public close(): void {
        this.wrapper.close();
    }

    public isConnected(): boolean {
        return this.wrapper.isConnected();
    }

    public clientData(): ClientData {
        return this.clientDataManagement;
    }

    public errorMessage(): string {
        return this.wrapper.lastError();
    }
}
