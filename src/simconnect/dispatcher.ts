import { Connection } from './connection';
import { ClientDataPeriod, ClientDataRequest, SimulatorDataPeriod } from './constants';
import { ClientDataArea, SimulatorDataArea } from './dataareas';
import { SystemEvent } from './events';
import { DispatcherResponse } from './types';

const simconnect = require('./libs/simconnect');

export interface Dispatcher {
    requestClientData(clientData: ClientDataArea, period: ClientDataPeriod, request: ClientDataRequest): boolean;
    requestSimulatorData(simulatorData: SimulatorDataArea, period: SimulatorDataPeriod): boolean;
    subscribeSystemEvent(systemEvent: SystemEvent): boolean;
    unsubscribeSystemEvent(systemEvent: SystemEvent): boolean;
    nextDispatch(): DispatcherResponse;
    lastError(): string;
}

export const Dispatcher: {
    new(connection: Connection): Dispatcher
} = simconnect.DispatcherBindings;
