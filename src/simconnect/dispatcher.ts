import { Connection } from './connection';

const simconnect = require('./libs/simconnect');

export interface Dispatcher {
    nextDispatch(): object;
    lastError(): string;
}

export const Dispatcher: {
    new(connection: Connection): Dispatcher
} = simconnect.DispatcherBindings;
