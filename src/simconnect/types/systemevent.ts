export enum SystemEventObjectType {
    User = 0,
    All = 1,
    Aircraft = 2,
    Helicopter = 3,
    Boat = 4,
    Ground = 5,
}

export enum SystemEventPauseType {
    Unpaused = 0,
    FullPause = 1,
    PauseWithSound = 2,
    ActivePause = 4,
    SimPause = 8,
}

export enum SystemEventViewType {
    Cockpit2D = 1,
    CockpitVirtual = 2,
    Orthogonal = 4,
}

export interface SystemEventFilename {
    filename: string;
}

export interface SystemEventAiObject {
    id: number;
    type: SystemEventObjectType;
}

export interface SystemEventPause {
    type: SystemEventPauseType;
}

export interface SystemEventSim {
    running: boolean;
}

export interface SystemEventSound {
    soundSwitchOn: boolean;
}

export interface SystemEventView {
    view: SystemEventViewType;
}

export interface SystemEventMessage {
    eventId: number;
    content?: SystemEventFilename | SystemEventAiObject | SystemEventPause | SystemEventSim | SystemEventSound | SystemEventView;
}
