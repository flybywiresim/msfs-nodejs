import { ClientDataArea } from './clientdataarea';
import { ClientDataPeriod, ClientDataRequest } from './constants';
import { Connection } from './connection';
import { DispatcherResponse } from './types';

const simconnect = require('./libs/simconnect');

export interface Dispatcher {
    requestClientData(clientData: ClientDataArea, period: ClientDataPeriod, request: ClientDataRequest): boolean;
    nextDispatch(): DispatcherResponse;
    lastError(): string;
}

export const Dispatcher: {
    new(connection: Connection): Dispatcher
} = simconnect.DispatcherBindings;
