export class Dispatcher {
    private timer: NodeJS.Timer = null;

    public start(): void {
        if (this.timer === null) {
            this.timer = setInterval(() => this.nextDispatch(), 50);
        }
    }

    public stop(): void {
        if (this.timer !== null) {
            clearInterval(this.timer);
            this.timer = null;
        }
    }

    private nextDispatch(): void {

    }
}
