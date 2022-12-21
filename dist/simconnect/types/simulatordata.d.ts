import { SimulatorDataType, SimulatorDataWaypointFlags } from '../constants';
export interface SimulatorDataDefinition {
    type: SimulatorDataType;
    name: string;
    unit: string;
    memberName: string;
}
export interface SimulatorDataRequestMessage {
    definitionId: number;
    content: object;
}
export interface SimulatorDataInitPosition {
    latitude: number;
    longitude: number;
    altitude: number;
    pitch: number;
    bank: number;
    heading: number;
    onGround: boolean;
    airspeed: number;
}
export interface SimulatorDataMarkerState {
    name: 'Cg' | 'ModelCenter' | 'Wheel' | 'Skid' | 'Ski' | 'Float' | 'Scrape' | 'Engine' | 'Prop' | 'Eyepoint' | 'LongScale' | 'LatScale' | 'VertScale' | 'AeroCenter' | 'WingApex' | 'RefChord' | 'Datum' | 'WingTip' | 'FuelTank' | 'Forces';
    isOn: boolean;
}
export interface SimulatorDataWaypoint {
    latitude: number;
    longitude: number;
    altitude: number;
    flags: number | SimulatorDataWaypointFlags;
    speedInKnots: number;
    percentThrottle?: number;
}
export interface SimulatorDataLatLongAlt {
    latitude: number;
    longitude: number;
    altitude: number;
}
export interface SimulatorDataXYZ {
    x: number;
    y: number;
    z: number;
}
