const msfs = require('build/Release/msfs');

export class Connection {
    public open(name: string): void {
        msfs.openSimConnect(name);
    }

    public close(): void {
        msfs.closeSimConnect();
    }
}
