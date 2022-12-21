export const ClientDataOffsetAuto = -1;

export const ClientDataMaxSize = 8192;

export enum ClientDataType {
    Int8 = -1,
    Int16 = -2,
    Int32 = -3,
    Int64 = -4,
    Float32 = -5,
    Float64 = -6
}

export enum SimulatorDataType {
    Invalid = 0,
    Int32 = 1,
    Int64 = 2,
    Float32 = 3,
    Float64 = 4,
    String8 = 5,
    String32 = 6,
    String64 = 7,
    String128 = 8,
    String256 = 9,
    String260 = 10,
    StringVariable = 11,
    InitPosition = 12,
    MarkerState = 13,
    Waypoint = 14,
    LatLongAlt = 15,
    XYZ = 16,
}

export enum ClientDataPeriod {
    Never = 0,
    Once = 1,
    VisualFrame = 2,
    OnSet = 3,
    Second = 4,
}

export enum SimulatorDataPeriod {
    Never = 0,
    Once = 1,
    VisualFrame = 2,
    SimFrame = 3,
    Second = 4,
}

export enum SimulatorDataWaypointFlags {
    SpeedRequested = 0x04,
    ThrottleRequested = 0x08,
    ComputeVerticalSpeed = 0x10,
    AltitudeIsAGL = 0x20,
    OnGround = 0x00100000,
    Reverse = 0x00200000,
    WrapToFirst = 0x00400000,
}

export enum ClientDataRequest {
    Default = 0,
    Changed = 1,
    Tagged = 2,
}
