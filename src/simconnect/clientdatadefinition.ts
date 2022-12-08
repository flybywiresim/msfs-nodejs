export class ClientDataDefinition {
    readonly dataDefinitionId: number;

    readonly offset: number;

    readonly sizeOrType: number;

    readonly epsilon: number;

    constructor(dataDefinition: number, offset: number, sizeOrType: number, epsilon: number = 0.0) {
        this.dataDefinitionId = dataDefinition;
        this.offset = offset;
        this.sizeOrType = sizeOrType;
        this.epsilon = epsilon;
    }
}
