export interface ClientDataDefinition {
    definitionId: number;
    offset: number;
    sizeOrType: number;
    epsilon: number;
    memberName: string;
}
export interface ClientDataRequestMessage {
    clientDataId: number;
    content: object;
}
