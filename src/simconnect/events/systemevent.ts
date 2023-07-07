import { Connection } from '../connection';
import { SystemEventType } from '../constants';

const simconnect = require('./libs/simconnect');

export interface SystemEvent {
    lastError(): string;
}

export const SystemEvent: {
    new(connection: Connection, systemEventId: number, eventType: SystemEventType): SystemEvent,
} = simconnect.SystemEventBindings;
