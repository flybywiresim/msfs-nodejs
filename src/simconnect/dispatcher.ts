import { Connection } from './connection';
import { DispatcherResponse } from './types';

const simconnect = require('./libs/simconnect');

export interface Dispatcher {
    nextDispatch(): DispatcherResponse;
    lastError(): string;
}

export const Dispatcher: {
    new(connection: Connection): Dispatcher
} = simconnect.DispatcherBindings;
