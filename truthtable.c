#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct variable Variable;

struct variable
{
    char *name;
    int value;
};

typedef struct variableNode VariableNode;

struct variableNode
{
    struct variable data;
    int index;
    struct variableNode *next;
};

typedef enum
{
    AND,
    OR,
    NAND,
    NOR,
    XOR,
    NOT,
    PASS,
    DECODER,
    MULTIPLEXER
} kind_t;

typedef struct gateNode GateNode;

struct gateNode
{
    kind_t operationType;
    int inputCount;
    int outputCount;
    int selectorCount;
    int *inputIndices;
    int *outputIndices;
    int *selectorIndices;
    GateNode *next;
};

struct circuit
{
    int inputCount;
    int outputCount;
    int zeroIndex;
    int oneIndex;
    int dashIndex;
    int *outputIndices;
    GateNode *firstGate;
    GateNode *lastGate;
    VariableNode *firstVariable;
    VariableNode *lastVariable;
    Variable *variableArray;
};

struct circuit globalCircuit;

void convertVariableListToArray()
{
    int variablecount = globalCircuit.lastVariable->index + 1;
    struct variable *variableArray = (Variable *)malloc(sizeof(Variable) * variablecount);
    VariableNode *ptr = globalCircuit.firstVariable;
    ;

    for (int i = 0; i < variablecount; i++)
    {
        if (ptr == NULL)
        {
            printf("Unexpected error, pointer is null at index = %d.\n", i);
            break;
        }
        variableArray[i].name = ptr->data.name;
        variableArray[i].value = ptr->data.value;
        ptr = ptr->next;
    }

    globalCircuit.variableArray = variableArray;
    globalCircuit.variableArray[globalCircuit.zeroIndex].value = 0;
    globalCircuit.variableArray[globalCircuit.oneIndex].value = 1;
}

int search(char *name)
{
    if (name == NULL)
    {
        return -1;
    }

    if (globalCircuit.firstVariable == NULL)
    {
        return -1;
    }

    VariableNode *ptr = globalCircuit.firstVariable;
    while (ptr != NULL)
    {
        if (strcmp(ptr->data.name, name) == 0)
        {
            return ptr->index;
        }
        ptr = ptr->next;
    }
    return -1;
}

VariableNode *insertInVarList(VariableNode *new)
{
    VariableNode *prev = globalCircuit.lastVariable;
    new->next = NULL;
    if (prev == NULL)
    {
        new->index = 0;
        globalCircuit.firstVariable = new;
        globalCircuit.lastVariable = new;
        return new;
    }

    new->index = prev->index + 1;
    prev->next = new;

    globalCircuit.lastVariable = new;
    return new;
}

void insertInGateList(GateNode *new)
{
    GateNode *prev = globalCircuit.lastGate;
    new->next = NULL;
    if (prev == NULL)
    {
        globalCircuit.firstGate = new;
        globalCircuit.lastGate = new;
        return;
    }

    prev->next = new;
    globalCircuit.lastGate = new;
}

int searchOrInsert(char *name)
{

    int index = search(name);
    if (index != -1)
    {
        free(name);
        name = NULL;
        return index;
    }

    VariableNode *newVar = (VariableNode *)malloc(sizeof(VariableNode));
    newVar->data.name = name;
    VariableNode *last = insertInVarList(newVar);
    return last->index;
}

void doDecoder(GateNode *gate)
{
    if (gate->operationType != DECODER)
    {
        return;
    }

    if (globalCircuit.variableArray == NULL)
    {
        fprintf(stderr, "Error: Missing variable nodes for DECODER gate.\n");
        return;
    }

    int outputIndex = 0;
    // for (int i = data->inputCount - 1; i >= 0; i-- )
    for (int i = 0; i < gate->inputCount; i++)
    {
        int inputValue = globalCircuit.variableArray[gate->inputIndices[i]].value;
        outputIndex <<= 1;
        outputIndex += inputValue;
        // printf("%d %d \n", outputIndex, inputValue);
    }

    for (int i = 0; i < gate->outputCount; i++)
    {
        if (i == outputIndex)
        {
            globalCircuit.variableArray[gate->outputIndices[i]].value = 1;
        }
        else
        {
            globalCircuit.variableArray[gate->outputIndices[i]].value = 0;
        }
    }
}

void doMultiplexer(GateNode *gate)
{
    int inputIndex = 0;

    for (int i = 0; i < gate->selectorCount; i++)
    {
        int selectorValue = globalCircuit.variableArray[gate->selectorIndices[i]].value;
        inputIndex <<= 1;
        inputIndex += selectorValue;
    }

    int output = globalCircuit.variableArray[gate->inputIndices[inputIndex]].value;
    globalCircuit.variableArray[gate->outputIndices[0]].value = output;
}

void doOperation(GateNode *gate)
{

    if (globalCircuit.variableArray == NULL)
    {
        fprintf(stderr, "Error: Missing variable nodes for AND gate.\n");
    }

    int var1index = gate->inputIndices[0];
    int var2index = 0;
    int outputIndex = gate->outputIndices[0];
    int result = 0;

    if (gate->inputCount > 1)
    {
        var2index = gate->inputIndices[1];
    }

    switch (gate->operationType)
    {
    case AND:
        result = globalCircuit.variableArray[var1index].value && globalCircuit.variableArray[var2index].value;
        break;
    case OR:
        result = globalCircuit.variableArray[var1index].value || globalCircuit.variableArray[var2index].value;
        break;
    case NAND:
        result = !(globalCircuit.variableArray[var1index].value && globalCircuit.variableArray[var2index].value);
        break;
    case NOR:
        result = !(globalCircuit.variableArray[var1index].value || globalCircuit.variableArray[var2index].value);
        break;
    case XOR:
        result = globalCircuit.variableArray[var1index].value != globalCircuit.variableArray[var2index].value;
        break;
    case NOT:
        result = !(globalCircuit.variableArray[var1index].value);
        break;
    case PASS:
        result = globalCircuit.variableArray[var1index].value;
        break;
    case DECODER:
        doDecoder(gate);
        return;
    case MULTIPLEXER:
        doMultiplexer(gate);
        return;
    default:
        printf("Not implemented");
    }
    globalCircuit.variableArray[outputIndex].value = result;
}

void printTruthTableRow()
{
    for (int i = 0; i < globalCircuit.inputCount; i++)
    {
        printf("%d ", globalCircuit.variableArray[i].value);
    }
    printf("|");
    for (int j = 0; j < globalCircuit.outputCount; j++)
    {
        int index = globalCircuit.outputIndices[j];
        printf(" %d", globalCircuit.variableArray[index].value);
    }
    printf("\n");
}

int *getInputArray(int count, int n)
{
    int *resultArray = (int *)malloc(sizeof(int) * count);
    int value = n;
    for (int i = count - 1; i >= 0; i--)
    {
        resultArray[i] = value % 2;
        value /= 2;
    }
    return resultArray;
}

kind_t findKind(char *gateName)
{
    if (strcmp(gateName, "AND") == 0)
    {
        return AND;
    }
    else if (strcmp(gateName, "OR") == 0)
    {
        return OR;
    }
    else if (strcmp(gateName, "NAND") == 0)
    {
        return NAND;
    }
    else if (strcmp(gateName, "NOR") == 0)
    {
        return NOR;
    }
    else if (strcmp(gateName, "XOR") == 0)
    {
        return XOR;
    }
    else if (strcmp(gateName, "NOT") == 0)
    {
        return NOT;
    }
    else if (strcmp(gateName, "PASS") == 0)
    {
        return PASS;
    }
    else if (strcmp(gateName, "DECODER") == 0)
    {
        return DECODER;
    }
    else if (strcmp(gateName, "MULTIPLEXER") == 0)
    {
        return MULTIPLEXER;
    }
    printf("Unknown gate. %s\n", gateName);
    exit(1);
}

void readBinaryOp(FILE *fptr, kind_t operationType)
{
    char *input1 = (char *)malloc(sizeof(char) * 17);
    char *input2 = (char *)malloc(sizeof(char) * 17);
    char *output = (char *)malloc(sizeof(char) * 17);
    fscanf(fptr, " %16s %16s %16s", input1, input2, output);
    GateNode *newGate = (GateNode *)malloc(sizeof(GateNode));
    newGate->operationType = operationType;
    newGate->inputCount = 2;
    newGate->outputCount = 1;
    newGate->inputIndices = (int *)malloc(sizeof(int) * 2);
    newGate->outputIndices = (int *)malloc(sizeof(int));

    newGate->inputIndices[0] = searchOrInsert(input1);
    newGate->inputIndices[1] = searchOrInsert(input2);
    newGate->outputIndices[0] = searchOrInsert(output);

    insertInGateList(newGate);
}

void readUnaryOp(FILE *fptr, kind_t operationType)
{
    char *input = (char *)malloc(sizeof(char) * 17);
    char *output = (char *)malloc(sizeof(char) * 17);
    fscanf(fptr, " %16s %16s", input, output);
    GateNode *newGate = (GateNode *)malloc(sizeof(GateNode));
    newGate->operationType = operationType;
    newGate->inputCount = 1;
    newGate->outputCount = 1;
    newGate->inputIndices = (int *)malloc(sizeof(int));
    newGate->outputIndices = (int *)malloc(sizeof(int));

    newGate->inputIndices[0] = searchOrInsert(input);
    newGate->outputIndices[0] = searchOrInsert(output);

    insertInGateList(newGate);
}

void readDecoder(FILE *fptr, kind_t operationType)
{

    int numInputs;
    fscanf(fptr, " %d", &numInputs);
    int power = 1;
    for (int i = 1; i <= numInputs; i++)
    {
        power *= 2;
    }
    int numOutputs = power;
    GateNode *newGate = (GateNode *)malloc(sizeof(GateNode));
    newGate->operationType = operationType;
    newGate->inputCount = numInputs;
    newGate->outputCount = numOutputs;
    newGate->inputIndices = (int *)malloc(sizeof(int) * numInputs);
    newGate->outputIndices = (int *)malloc(sizeof(int) * numOutputs);

    for (int i = 0; i < numInputs; i++)
    {
        char *input1 = (char *)malloc(sizeof(char) * 17);
        fscanf(fptr, " %16s", input1);
        newGate->inputIndices[i] = searchOrInsert(input1);
    }

    for (int i = 0; i < numOutputs; i++)
    {
        char *output = (char *)malloc(sizeof(char) * 17);
        fscanf(fptr, " %16s", output);
        newGate->outputIndices[i] = searchOrInsert(output);
    }

    insertInGateList(newGate);
}

void readMultiplexer(FILE *fptr, kind_t operationType)
{
    int numSelectors;
    fscanf(fptr, " %d", &numSelectors);
    int power = 1;
    for (int i = 1; i <= numSelectors; i++)
    {
        power *= 2;
    }
    int numInputs = power;
    GateNode *newGate = (GateNode *)malloc(sizeof(GateNode));
    newGate->operationType = operationType;
    newGate->inputCount = numInputs;
    newGate->outputCount = 1;
    newGate->selectorCount = numSelectors;
    newGate->inputIndices = (int *)malloc(sizeof(int) * numInputs);
    newGate->outputIndices = (int *)malloc(sizeof(int));
    newGate->selectorIndices = (int *)malloc(sizeof(int) * numSelectors);

    for (int i = 0; i < numInputs; i++)
    {
        char *input1 = (char *)malloc(sizeof(char) * 17);
        fscanf(fptr, " %16s", input1);
        newGate->inputIndices[i] = searchOrInsert(input1);
    }

    for (int i = 0; i < numSelectors; i++)
    {
        char *selector = (char *)malloc(sizeof(char) * 17);
        fscanf(fptr, " %16s", selector);
        newGate->selectorIndices[i] = searchOrInsert(selector);
    }

    char *output = (char *)malloc(sizeof(char) * 17);
    fscanf(fptr, " %16s", output);
    newGate->outputIndices[0] = searchOrInsert(output);

    insertInGateList(newGate);
}

void buildCircuit(char *filename)
{
    if (filename == NULL)
    {
        filename = "/dev/stdin";
    }
    FILE *fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        printf("Error creating %s\n", filename);
        exit(1);
    }

    char *input = (char *)malloc(sizeof(char) * 17);
    int inputCount;

    fscanf(fptr, "%16s %d", input, &inputCount);
    free(input);
    globalCircuit.inputCount = inputCount;
    VariableNode *last = globalCircuit.firstVariable;

    for (int i = 0; i < inputCount; i++)
    {
        VariableNode *newVar = (VariableNode *)malloc(sizeof(VariableNode));
        char *name = (char *)malloc(sizeof(char) * 17);

        fscanf(fptr, "%16s", name);
        newVar->data.name = name;
        last = insertInVarList(newVar);
    }

    VariableNode *zeroVar = (VariableNode *)malloc(sizeof(VariableNode));
    zeroVar->data.name = "0";
    last = insertInVarList(zeroVar);
    globalCircuit.zeroIndex = last->index;

    VariableNode *oneVar = (VariableNode *)malloc(sizeof(VariableNode));
    oneVar->data.name = "1";
    last = insertInVarList(oneVar);
    globalCircuit.oneIndex = last->index;

    VariableNode *dash = (VariableNode *)malloc(sizeof(VariableNode));
    dash->data.name = "_";
    last = insertInVarList(dash);
    globalCircuit.dashIndex = last->index;

    char *output = (char *)malloc(sizeof(char) * 17);
    int outputCount;

    fscanf(fptr, "%16s %d", output, &outputCount);
    free(output);
    globalCircuit.outputCount = outputCount;
    int *outputIndices = (int *)malloc(sizeof(int) * outputCount);

    for (int i = 0; i < outputCount; i++)
    {
        VariableNode *newVar = (VariableNode *)malloc(sizeof(VariableNode));
        char *oname = (char *)malloc(sizeof(char) * 17);
        fscanf(fptr, "%16s", oname);
        newVar->data.name = oname;
        last = insertInVarList(newVar);
        outputIndices[i] = last->index;
    }

    globalCircuit.outputIndices = outputIndices;

    int moreData = 1;
    while (moreData)
    {

        char *gateName = (char *)malloc(sizeof(char) * 17);
        moreData = fscanf(fptr, " %16s", gateName);
        if (moreData <= 0)
        {
            free(gateName);
            break;
        }
        kind_t gateKind = findKind(gateName);
        free(gateName);
        switch (gateKind)
        {
        case AND:
        case OR:
        case NAND:
        case NOR:
        case XOR:
            readBinaryOp(fptr, gateKind);
            break;
        case NOT:
        case PASS:
            readUnaryOp(fptr, gateKind);
            break;
        case DECODER:
            readDecoder(fptr, gateKind);
            break;
        case MULTIPLEXER:
            readMultiplexer(fptr, gateKind);
            break;
        default:
            printf("Not implemented yet");
            exit(1);
        }
    }

}

void printTruthTable()
{
    int inputCount = globalCircuit.inputCount;
    int power = 1;

    for (int i = 1; i <= inputCount; i++)
    {
        power *= 2;
    }

    for (int i = 0; i < power; i++)
    {
        int *inputArray = getInputArray(inputCount, i);

        for (int j = 0; j < inputCount; j++)
        {
            globalCircuit.variableArray[j].value = inputArray[j];
        }

        for (GateNode *ptr = globalCircuit.firstGate; ptr != NULL; ptr = ptr->next)
        {
            doOperation(ptr);
        }
        printTruthTableRow();
        free(inputArray);
    }
}

int main(int argc, char **argv)
{
    //"../data/test.1.03.txt";
    if (argc < 2)
    {
        printf("No arguments");
        exit(1);
    }
    else
    {
        buildCircuit(argv[1]);
    }
    convertVariableListToArray();
    printTruthTable();

    // Free the variable linked list
    VariableNode *ptr = globalCircuit.firstVariable;
    while (ptr != NULL)
    {
        VariableNode *prev = ptr;
        ptr = ptr->next;

        if (strcmp(prev->data.name, "0") != 0 && strcmp(prev->data.name, "1") != 0 && strcmp(prev->data.name, "_") != 0 && prev->data.name != NULL)
        {
            free(prev->data.name);
        }

        free(prev);
    }
    globalCircuit.firstVariable = NULL;

    // Free the gate linked list
    GateNode *pt = globalCircuit.firstGate;
    while (pt != NULL)
    {
        GateNode *pre = pt;
        pt = pt->next;

        if (pre->inputIndices != NULL)
        {
            free(pre->inputIndices);
        }

        if (pre->outputIndices != NULL)
        {
            free(pre->outputIndices);
        }

        if (pre->operationType == MULTIPLEXER)
        {
            free(pre->selectorIndices);
        }

        free(pre); // Free the GateNode itself
    }
    globalCircuit.firstGate = NULL;

    if (globalCircuit.variableArray != NULL)
    {   
        free(globalCircuit.variableArray);
        globalCircuit.variableArray = NULL;
    }

    if (globalCircuit.outputIndices != NULL)
    {
        free(globalCircuit.outputIndices);
        globalCircuit.outputIndices = NULL;
    }

    return EXIT_SUCCESS;
}
