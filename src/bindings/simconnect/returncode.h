namespace msfs {
namespace nodejs {
namespace simconnect {
    enum ReturnCode {
        Ok = 0,
        NotInitialized = 1,
        NotConnected = 2,
        AlreadyConnected = 3,
        Forbidden = 4,
        Failure = 5,
        InvalidArgument = 6,
        AlreadyCreated = 7,
        DuplicateId = 8,
    };
}
}
}
