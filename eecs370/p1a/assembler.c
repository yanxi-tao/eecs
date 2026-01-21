/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000
#define OP_CNT 8
#define DIR_CNT 1

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);
static int endsWith(char *, char *);

// helper
void abortWithMsg(const char *msg) {
    fprintf(stderr, "ErrAbort: %s", msg);
    exit(1);
}

//pass 1
typedef struct {
    char name[7];
    uint32_t addr;
} Label;

typedef struct {
    Label value[MAXLINELENGTH];
    size_t size;
} Labels;

Labels labels = { .size = 0 };

void addLabel(const char * label, uint32_t addr) {
    if (label[0] == '\0') return; // early return if no label

    // check duplicate
    for (size_t i = 0; i < labels.size; ++i) {
        if (strcmp(labels.value[i].name, label) == 0) {
            abortWithMsg("Deuplicate or Invalid Label");
            return;
        }
    }

    // update labels
    memcpy(labels.value[labels.size].name, label, 7);
    labels.value[labels.size].addr = addr;
    ++labels.size;
    return;
}

const Label* lookup_label(const char * label) {
    for (size_t i = 0; i < labels.size; ++i) {
        if (strcmp(labels.value[i].name, label) == 0) {
            return &labels.value[i];
        }
    }
    // abortWithMsg("Undefined Label");

    return NULL;
}

//pass 2

typedef enum {
    // ISA ops
    OP_ADD  = 0, // 000
    OP_NOR  = 1, // 001
    OP_LW   = 2, // 010
    OP_SW   = 3, // 011
    OP_BEQ  = 4, // 100
    OP_JALR = 5, // 101
    OP_HALT = 6, // 110
    OP_NOOP = 7, // 111

    // ISA directive
    DIR_FILL = 8,

    // other
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

    // directive
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

uint32_t assembleRType(Opcode op, const char *ra, const char *rb, const char *rd) {
    if (!isValidReg(ra) || !isValidReg(rb) || !isValidReg(rd)) abortWithMsg("Invalid Register");

    uint32_t instruct = 0;
    instruct |= ((uint32_t)op & 0x7) << 22;
    instruct |= ((uint32_t)atoi(ra) & 0x7) << 19;
    instruct |= ((uint32_t)atoi(rb) & 0x7) << 16;
    instruct |= ((uint32_t)atoi(rd) & 0x7);

    return instruct;
}

uint32_t assembleIType(Opcode op, const char *ra, const char *rb, const char *offset, uint32_t pc) {
    if (!isValidReg(ra) || !isValidReg(rb)) abortWithMsg("Invalid Register");

    uint32_t instruct = 0;
    instruct |= ((uint32_t)op & 0x7) << 22;
    instruct |= ((uint32_t)atoi(ra) & 0x7) << 19;
    instruct |= ((uint32_t)atoi(rb) & 0x7) << 16;

    int32_t calculatedOffset = 0;

    if (isNumber((char *)offset)) {
        int32_t val = strtol(offset, NULL, 0);
        if (val < INT16_MIN || val > INT16_MAX) abortWithMsg("Offset Overflow");
        instruct |= ((uint32_t)val & 0xffff);
    } else {
        const Label* label = lookup_label(offset);
        if (!label) abortWithMsg("Undefined Label");
        switch (op) {
            case OP_LW:
            case OP_SW:
                calculatedOffset = label->addr;
                break;
            case OP_BEQ:
                calculatedOffset = (int64_t)label->addr - (int64_t)pc - 1;
                if (calculatedOffset < INT16_MIN || calculatedOffset > INT16_MAX) {
                    abortWithMsg("Branch offset too large");
                }
                break;
            default:
                abortWithMsg("Unreachable: Cannot use a label with this opcode");
                break;
        }
        instruct |= ((uint32_t)calculatedOffset & 0xffff);
    }

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

uint32_t assembleFill(Opcode op, const char *val) {
    if (!isNumber((char *)val)) {
        const Label* label = lookup_label(val);
        if (!label) abortWithMsg("Invalid Fill Value (Undefined Label)");

        return (uint32_t)label->addr;
    }

    long long data = strtoll(val, NULL, 0);

    if (data < INT32_MIN || data > UINT32_MAX) {
        abortWithMsg("Fill Value Out of Range");
    }
    return (uint32_t)data;
}

int
main(int argc, char **argv)
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    if (!endsWith(inFileString, ".as") &&
        !endsWith(inFileString, ".s") &&
        !endsWith(inFileString, ".lc2k")
    ) {
        printf("warning: assembly code file does not end with .as, .s, or .lc2k\n");
    }

    if (!endsWith(outFileString, ".mc")) {
        printf("error: machine code file must end with .mc\n");
        exit(1);
    }

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    //pass 1
    uint32_t line_num = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        // printf("lab: %s, op: %s, 0: %s, 1: %s, 2:%s\n", label, opcode, arg0, arg1, arg2);
        addLabel(label, line_num);
        ++line_num;
    }

    //pass 2
    rewind(inFilePtr);
    line_num = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        // printf("lab: %s, op: %s, 0: %s, 1: %s, 2:%s\n", label, opcode, arg0, arg1, arg2);
        Opcode op = get_op(opcode);
        if (op == OP_INVALID) abortWithMsg("Invalid Operation/Directive");

        uint32_t instruct;
        switch (op) {
            case OP_ADD:
            case OP_NOR:
                instruct = assembleRType(op, arg0, arg1, arg2);
                break;
            case OP_LW:
            case OP_SW:
            case OP_BEQ:
                instruct = assembleIType(op, arg0, arg1, arg2, line_num);
                break;
            case OP_JALR:
                instruct = assembleJType(op, arg0, arg1);
                break;
            case OP_HALT:
            case OP_NOOP:
                instruct = assembleOType(op);
                break;
            case DIR_FILL:
                instruct = assembleFill(op, arg0);
                break;
            default:
                abortWithMsg("Unreachable");
                return 1;

        }
        printHexToFile(outFilePtr, instruct);
        ++line_num;
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

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr) {
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for(int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address) {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH-1) {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
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


/*
* NOTE: The code defined below is not to be modifed as it is implimented correctly.
*/

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
	printf("error: line too long\n");
	exit(1);
    }

    // Ignore blank lines at the end of the file.
    if(lineIsBlank(line)) {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

static inline int
isNumber(char *string)
{
    int num;
    char c;
    return((sscanf(string, "%d%c",&num, &c)) == 1);
}


// Prints a machine code word in the proper hex format to the file
static inline void
printHexToFile(FILE *outFilePtr, int word) {
    fprintf(outFilePtr, "0x%08X\n", word);
}

// Returns 1 if string ends with substr, 0 otherwise
static int
endsWith(char *string, char *substr) {
    size_t stringLen = strlen(string);
    size_t substrLen = strlen(substr);
    if (stringLen < substrLen) {
        return 0; // string too short
    }
    char *stringEnd = string + stringLen - substrLen;
    if (strcmp(stringEnd, substr) == 0) {
        return 1;
    }
    return 0;
}
