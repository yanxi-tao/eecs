#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define OP_CNT 8
#define DIR_CNT 1

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static void checkForInstructionsAfterFill(FILE *inFilePtr);
static inline int isNumber(char *);

// Helper
void abortWithMsg(const char *msg) {
    fprintf(stderr, "ErrAbort: %s\n", msg);
    exit(1);
}

// Check label types
bool isGlobalLabel(const char *label) {
    return (label[0] >= 'A' && label[0] <= 'Z');
}

bool isLocalLabel(const char *label) {
    return (label[0] >= 'a' && label[0] <= 'z');
}

// --- Data Structures for Object File ---

typedef struct {
    char name[7];
    char type;      // 'T' (Text), 'D' (Data), 'U' (Undefined)
    int offset;     // Offset relative to section start
} SymbolTableEntry;

typedef struct {
    int offset;
    char opcode[7];
    char label[7];
} RelocationEntry;

SymbolTableEntry symbolTable[MAXLINELENGTH];
int symbolTableSize = 0;

RelocationEntry relocationTable[MAXLINELENGTH];
int relocationTableSize = 0;

uint32_t textSegment[MAXLINELENGTH];
int textSize = 0;

uint32_t dataSegment[MAXLINELENGTH];
int dataSize = 0;

// --- Internal Label Storage (Pass 1) ---

typedef struct {
    char name[7];
    int offset;     // Offset relative to section start
    char section;   // 'T' or 'D'
} DefinedLabel;

DefinedLabel localLabels[MAXLINELENGTH];
int localLabelCount = 0;

void addDefinedLabel(const char *label, int offset, char section) {
    if (label[0] == '\0') return;

    // Check duplicate
    for (int i = 0; i < localLabelCount; ++i) {
        if (strcmp(localLabels[i].name, label) == 0) {
            abortWithMsg("Duplicate Label Definition");
        }
    }

    strcpy(localLabels[localLabelCount].name, label);
    localLabels[localLabelCount].offset = offset;
    localLabels[localLabelCount].section = section;
    localLabelCount++;
}

DefinedLabel* findDefinedLabel(const char *label) {
    for (int i = 0; i < localLabelCount; ++i) {
        if (strcmp(localLabels[i].name, label) == 0) {
            return &localLabels[i];
        }
    }
    return NULL;
}

// Adds a symbol to the output table if it doesn't already exist
void addSymbolToTable(const char *name, char type, int offset) {
    for (int i = 0; i < symbolTableSize; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return; // Already exists
        }
    }
    strcpy(symbolTable[symbolTableSize].name, name);
    symbolTable[symbolTableSize].type = type;
    symbolTable[symbolTableSize].offset = offset;
    symbolTableSize++;
}

// Adds a relocation entry
void addRelocation(int offset, const char *opcode, const char *label) {
    relocationTable[relocationTableSize].offset = offset;
    strcpy(relocationTable[relocationTableSize].opcode, opcode);
    strcpy(relocationTable[relocationTableSize].label, label);
    relocationTableSize++;
}

// --- Opcodes ---

typedef enum {
    OP_ADD  = 0,
    OP_NOR  = 1,
    OP_LW   = 2,
    OP_SW   = 3,
    OP_BEQ  = 4,
    OP_JALR = 5,
    OP_HALT = 6,
    OP_NOOP = 7,
    DIR_FILL = 8,
    OP_INVALID = -1
} Opcode;

typedef struct {
    const char *mnemonic;
    Opcode opcode;
} Op;

static const Op opTable[] = {
    {"add",  OP_ADD},
    {"nor",  OP_NOR},
    {"lw",   OP_LW},
    {"sw",   OP_SW},
    {"beq",  OP_BEQ},
    {"jalr", OP_JALR},
    {"halt", OP_HALT},
    {"noop", OP_NOOP},
    {".fill", DIR_FILL}
};

Opcode get_op(const char *str) {
    if (str == NULL) return OP_INVALID;
    for (size_t i = 0; i < OP_CNT + DIR_CNT; i++) {
        if (strcmp(str, opTable[i].mnemonic) == 0) {
            return opTable[i].opcode;
        }
    }
    return OP_INVALID;
}

bool isValidReg(const char *r) {
    if (!isNumber((char *)r) || atoi(r) < 0 || atoi(r) > 7) return false;
    return true;
}

// --- Assembly Functions ---

uint32_t assembleRType(Opcode op, const char *ra, const char *rb, const char *rd) {
    if (!isValidReg(ra) || !isValidReg(rb) || !isValidReg(rd)) abortWithMsg("Invalid Register");
    uint32_t instruct = 0;
    instruct |= ((uint32_t)op & 0x7) << 22;
    instruct |= ((uint32_t)atoi(ra) & 0x7) << 19;
    instruct |= ((uint32_t)atoi(rb) & 0x7) << 16;
    instruct |= ((uint32_t)atoi(rd) & 0x7);
    return instruct;
}

uint32_t assembleIType(Opcode op, const char *ra, const char *rb, int offset) {
    if (!isValidReg(ra) || !isValidReg(rb)) abortWithMsg("Invalid Register");
    if (offset < INT16_MIN || offset > INT16_MAX) abortWithMsg("Offset Overflow");

    uint32_t instruct = 0;
    instruct |= ((uint32_t)op & 0x7) << 22;
    instruct |= ((uint32_t)atoi(ra) & 0x7) << 19;
    instruct |= ((uint32_t)atoi(rb) & 0x7) << 16;
    instruct |= ((uint32_t)offset & 0xffff);
    return instruct;
}

uint32_t assembleJType(Opcode op, const char *ra, const char *rb) {
    if (!isValidReg(ra) || !isValidReg(rb)) abortWithMsg("Invalid Register");
    uint32_t instruct = 0;
    instruct |= ((uint32_t)op & 0x7) << 22;
    instruct |= ((uint32_t)atoi(ra) & 0x7) << 19;
    instruct |= ((uint32_t)atoi(rb) & 0x7) << 16;
    return instruct;
}

uint32_t assembleOType(Opcode op) {
    uint32_t instruct = 0;
    instruct |= ((uint32_t)op & 0x7) << 22;
    return instruct;
}

uint32_t assembleFill(int value) {
     return (uint32_t)value;
}

// --- Main ---

int main(int argc, char **argv) {
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <object-file>\n", argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    checkForBlankLinesInCode(inFilePtr);
    checkForInstructionsAfterFill(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    // --- PASS 1: Build Defined Label Table and Calculate Section Sizes ---
    int currentTextOffset = 0;
    int currentDataOffset = 0;
    bool inDataSection = false;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        // Determine section based on .fill
        if (!strcmp(opcode, ".fill")) {
            inDataSection = true;
        }

        if (label[0] != '\0') {
            if (inDataSection) {
                addDefinedLabel(label, currentDataOffset, 'D');
                // Defined Global labels in Data section go to Symbol Table
                if (isGlobalLabel(label)) {
                    addSymbolToTable(label, 'D', currentDataOffset);
                }
            } else {
                addDefinedLabel(label, currentTextOffset, 'T');
                // Defined Global labels in Text section go to Symbol Table
                if (isGlobalLabel(label)) {
                    addSymbolToTable(label, 'T', currentTextOffset);
                }
            }
        }

        if (inDataSection) {
            currentDataOffset++;
        } else {
            currentTextOffset++;
        }
    }

    // --- PASS 2: Generate Code and Tables ---
    rewind(inFilePtr);

    // Reset offsets for tracking
    int textPC = 0;
    int dataPC = 0;
    inDataSection = false;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        Opcode op = get_op(opcode);
        uint32_t instruct = 0;

        if (op == DIR_FILL) {
            inDataSection = true;
            int fillVal = 0;

            if (isNumber(arg0)) {
                long long data = strtoll(arg0, NULL, 0);
                if (data < INT32_MIN || data > UINT32_MAX) abortWithMsg("Fill Value Out of Range");
                fillVal = (int)data;
            } else {
                // Label used in .fill
                DefinedLabel* def = findDefinedLabel(arg0);

                if (def) {
                    // Defined (Local or Global)
                    fillVal = def->offset;

                    if (def->section == 'T') {
                        fillVal = def->offset;
                    } else {
                        fillVal = currentTextOffset + def->offset;
                    }

                    addRelocation(dataPC, ".fill", arg0);

                } else if (isGlobalLabel(arg0)) {
                    // Undefined Global
                    fillVal = 0;
                    addSymbolToTable(arg0, 'U', 0);
                    addRelocation(dataPC, ".fill", arg0);
                } else {
                     abortWithMsg("Undefined Local Label in .fill");
                }
            }
            instruct = assembleFill(fillVal);
            dataSegment[dataSize++] = instruct;
            dataPC++;

        } else {
            // Text Section Instructions
            switch (op) {
                case OP_ADD:
                case OP_NOR:
                    instruct = assembleRType(op, arg0, arg1, arg2);
                    break;
                case OP_JALR:
                    instruct = assembleJType(op, arg0, arg1);
                    break;
                case OP_HALT:
                case OP_NOOP:
                    instruct = assembleOType(op);
                    break;
                case OP_LW:
                case OP_SW:
                case OP_BEQ: {
                    int offsetVal = 0;
                    if (isNumber(arg2)) {
                        offsetVal = strtol(arg2, NULL, 0);
                    } else {
                        DefinedLabel* def = findDefinedLabel(arg2);

                        if (op == OP_BEQ) {
                            if (!def) {
                                abortWithMsg("beq label must be defined locally");
                            }

                            int targetAddr = (def->section == 'T') ? def->offset : (currentTextOffset + def->offset);
                            int pcAddr = textPC;
                            offsetVal = targetAddr - pcAddr - 1;

                        } else {
                            if (def) {
                                if (def->section == 'T') {
                                    offsetVal = def->offset;
                                } else {
                                    offsetVal = currentTextOffset + def->offset;
                                }
                                addRelocation(textPC, opcode, arg2);
                            } else if (isGlobalLabel(arg2)) {
                                offsetVal = 0;
                                addSymbolToTable(arg2, 'U', 0);
                                addRelocation(textPC, opcode, arg2);
                            } else {
                                abortWithMsg("Undefined Local Label");
                            }
                        }
                    }
                    instruct = assembleIType(op, arg0, arg1, offsetVal);
                    break;
                }
                default:
                    abortWithMsg("Unreachable");
            }
            textSegment[textSize++] = instruct;
            textPC++;
        }
    }

    // --- Output Object File ---

    // 1. Header
    fprintf(outFilePtr, "%d %d %d %d\n", textSize, dataSize, symbolTableSize, relocationTableSize);

    // 2. Text
    for (int i = 0; i < textSize; i++) {
        fprintf(outFilePtr, "0x%08X\n", textSegment[i]);
    }

    // 3. Data
    for (int i = 0; i < dataSize; i++) {
        fprintf(outFilePtr, "0x%08X\n", dataSegment[i]);
    }

    // 4. Symbol Table
    for (int i = 0; i < symbolTableSize; i++) {
        fprintf(outFilePtr, "%s %c %d\n", symbolTable[i].name, symbolTable[i].type, symbolTable[i].offset);
    }

    // 5. Relocation Table
    for (int i = 0; i < relocationTableSize; i++) {
        fprintf(outFilePtr, "%d %s %s\n", relocationTable[i].offset, relocationTable[i].opcode, relocationTable[i].label);
    }

    fclose(inFilePtr);
    fclose(outFilePtr);

    return(0);
}

// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line) {
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for(int line_idx=0; line_idx < strlen(line); ++line_idx) {
        int line_char_is_whitespace = 0;
        for(int whitespace_idx = 0; whitespace_idx < 4; ++ whitespace_idx) {
            if(line[line_idx] == whitespace[whitespace_idx]) {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if(!line_char_is_whitespace) {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }
        if(lineIsBlank(line)) {
            if(!blank_line_encountered) {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        } else {
            if(blank_line_encountered) {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}

static void checkForInstructionsAfterFill(FILE *inFilePtr) {
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
         arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    int fill_section_started = 0;
    int address_of_first_fill = 0;
    rewind(inFilePtr);

    for(int address = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); ++address) {
        if (!strcmp(opcode, ".fill")) {
            if (!fill_section_started) {
                fill_section_started = 1;
                address_of_first_fill = address;
            }
        } else {
            if (fill_section_started) {
                printf("Invalid Assembly: non-.fill instruction \"%s\" at address %d after .fill at address %d\n",
                       opcode, address, address_of_first_fill);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}

int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        return(0);
    }
    if (strlen(line) == MAXLINELENGTH-1) {
        printf("error: line too long\n");
        exit(1);
    }
    if(lineIsBlank(line)) {
        return 0;
    }

    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
        ptr += strlen(label);
    }

    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

static inline int isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}