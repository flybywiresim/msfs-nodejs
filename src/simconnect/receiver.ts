import { Connection } from './connection';
import { Dispatcher } from './dispatcher';
import { ErrorMessage, ExceptionMessage, OpenMessage } from './types';

export type ReceiverCallbacks = {
    open: (message: OpenMessage) => void;
    quit: () => void;
    exception: (message: ExceptionMessage) => void;
    error: (message: ErrorMessage) => void;
}

export class Receiver {
    private dispatcher: Dispatcher = null;

    private interval: NodeJS.Timer = null;

    private callbacks: ReceiverCallbacks = {
        open: null,
        quit: null,
        exception: null,
        error: null,
    }

    constructor(connection: Connection) {
        this.dispatcher = new Dispatcher(connection);
    }

    private processDispatch(): void {
        const response = this.dispatcher.nextDispatch();
        if (response === null) return;

        switch (response.type) {
        case 'open':
            if (this.callbacks.open !== null) this.callbacks.open(response.data as OpenMessage);
            break;
        case 'quit':
            if (this.callbacks.quit !== null) this.callbacks.quit();
            break;
        case 'exception':
            if (this.callbacks.exception !== null) this.callbacks.exception(response.data as ExceptionMessage);
            break;
        case 'error':
            if (this.callbacks.error !== null) this.callbacks.error(response.data as ErrorMessage);
            break;
        default:
            throw Error('Unknown message type in the dispatcher');
        }
    }

    public addCallback<K extends keyof ReceiverCallbacks>(event: K, callback: ReceiverCallbacks[K]): void {
        this.callbacks[event] = callback;
    }

    public start(): void {
        if (this.interval === null) {
            this.interval = setInterval(() => this.processDispatch(), 50);
        }
    }

    public stop(): void {
        if (this.interval !== null) {
            clearInterval(this.interval);
            this.interval = null;
        }
    }
}