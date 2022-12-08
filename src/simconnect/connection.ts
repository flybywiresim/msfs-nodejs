const msfs = require('./libs/msfs');

export class Connection {
    public async open(name: string): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            try {
                msfs.openSimConnect(name);
                resolve();
            } catch (err) {
                reject(err);
            }
        });
    }

    public close(): void {
        msfs.closeSimConnect();
    }
}
