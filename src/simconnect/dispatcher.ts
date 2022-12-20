import { ClientDataArea } from './clientdataarea';
import { ClientDataPeriod, ClientDataRequest, SimulatorDataPeriod } from './constants';
import { Connection } from './connection';
import { SimulatorDataArea } from './simulatordataarea';
import { DispatcherResponse } from './types';

const simconnect = require('./libs/simconnect');

export interface Dispatcher {
    requestClientData(clientData: ClientDataArea, period: ClientDataPeriod, request: ClientDataRequest): boolean;
    requestSimulatorData(simulatorData: SimulatorDataArea, period: SimulatorDataPeriod): boolean;
    nextDispatch(): DispatcherResponse;
    lastError(): string;
}

export const Dispatcher: {
    new(connection: Connection): Dispatcher
} = simconnect.DispatcherBindings;
