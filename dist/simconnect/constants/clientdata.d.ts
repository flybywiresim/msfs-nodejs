export declare const ClientDataOffsetAuto = -1;
export declare const ClientDataMaxSize = 8192;
export declare enum ClientDataType {
    Int8 = -1,
    Int16 = -2,
    Int32 = -3,
    Int64 = -4,
    Float32 = -5,
    Float64 = -6
}
export declare enum ClientDataPeriod {
    Never = 0,
    Once = 1,
    VisualFrame = 2,
    OnSet = 3,
    Second = 4
}
export declare enum ClientDataRequest {
    Default = 0,
    Changed = 1,
    Tagged = 2
}
