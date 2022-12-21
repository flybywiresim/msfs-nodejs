import { Connection } from '../connection';
import { SimulatorDataDefinition } from '../types';

const simconnect = require('./libs/simconnect');

export interface SimulatorDataArea {
    addDataDefinition(dataDefinition: SimulatorDataDefinition): boolean;
    lastError(): string;
}

export const SimulatorDataArea: {
    new(connection: Connection, simulatorDataId: number): SimulatorDataArea,
} = simconnect.SimulatorDataAreaBindings;
