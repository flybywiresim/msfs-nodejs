import { Connection } from '../connection';
import { SimulatorDataDefinition } from '../types';
export interface SimulatorDataArea {
    addDataDefinition(dataDefinition: SimulatorDataDefinition): boolean;
    lastError(): string;
}
export declare const SimulatorDataArea: {
    new (connection: Connection, simulatorDataId: number): SimulatorDataArea;
};
