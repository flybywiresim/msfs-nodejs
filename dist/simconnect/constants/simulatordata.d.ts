export declare enum SimulatorDataType {
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
    XYZ = 16
}
export declare enum SimulatorDataPeriod {
    Never = 0,
    Once = 1,
    VisualFrame = 2,
    SimFrame = 3,
    Second = 4
}
export declare enum SimulatorDataWaypointFlags {
    SpeedRequested = 4,
    ThrottleRequested = 8,
    ComputeVerticalSpeed = 16,
    AltitudeIsAGL = 32,
    OnGround = 1048576,
    Reverse = 2097152,
    WrapToFirst = 4194304
}
