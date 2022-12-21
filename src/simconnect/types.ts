import { SimulatorDataType, SimulatorDataWaypointFlags } from './constants';

export interface ClientDataDefinition {
    definitionId: number;
    offset: number;
    sizeOrType: number;
    epsilon: number;
    memberName: string;
}

export interface SimulatorDataDefinition {
    type: SimulatorDataType,
    name: string,
    unit: string,
    memberName: string,
}

export interface DispatcherResponse {
    type: 'open' | 'quit' | 'clientData' | 'simulatorData' | 'exception' | 'error';
    data: object;
}

export interface Version {
    major: number;
    minor: number;
}

export interface OpenMessage {
    application: {
        name: string;
        version: Version;
        build: Version;
    }
    simconnect: {
        version: Version;
        build: Version;
    }
}

export interface ExceptionMessage {
    exception: number;
    exceptionText: string;
    sendId: number;
    index: number;
}

export interface ErrorMessage {
    id: number;
    size: number;
    version: number;
}

export interface ClientDataRequestMessage {
    clientDataId: number;
    content: object;
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
    name: 'Cg' | 'ModelCenter' | 'Wheel' | 'Skid' | 'Ski' |
          'Float' | 'Scrape' | 'Engine' | 'Prop' | 'Eyepoint' |
          'LongScale' | 'LatScale' | 'VertScale' | 'AeroCenter' |
          'WingApex' | 'RefChord' | 'Datum' | 'WingTip' | 'FuelTank' |
          'Forces';
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
