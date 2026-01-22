/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure to NOT modify printState or any of the associated functions
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//DO NOT CHANGE THE FOLLOWING DEFINITIONS

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8 /*total number of machine registers [0,7]*/

// File Definitions
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct
stateStruct {
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
    int numInstructionsExecuted;
} stateType;

void printState(stateType *);

void printStats(stateType *);

static inline int convertNum(int32_t);

static int endsWith(char *, char *);

// -----------------

void abortWithMsg(const char *msg) {
    fprintf(stderr, "ErrAbort: %s", msg);
    exit(1);
}

// parse inst macro

#define OPCODE(inst) ((inst >> 22) & 0x7)
#define REG_A(inst) ((inst >> 19) & 0x7)
#define REG_B(inst) ((inst >> 16) & 0x7)
#define REG_DEST(inst) (inst & 0x7)
#define OFF_SET(inst) (inst & 0xffff)

int
main(int argc, char **argv)
{
    char line[MAXLINELENGTH];
    stateType state = {0};
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    if (!endsWith(argv[1], ".mc")) {
        printf("warning: machine code file does not end with .mc\n");
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s , please ensure you are providing the correct path", argv[1]);
        perror("fopen");
        exit(2);
    }


    /* read the entire machine-code file into memory */
    for (state.numMemory=0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
		    if (state.numMemory >= MEMORYSIZE) {
			      fprintf(stderr, "exceeded memory size\n");
			      exit(2);
		    }
		    if (sscanf(line, "%x", state.mem+state.numMemory) != 1) {
			      fprintf(stderr, "error in reading address %d\n", state.numMemory);
			      exit(2);
		    }
            printf("mem[ %d ] 0x%08X\n", state.numMemory, state.mem[state.numMemory]);
    }

    bool terminate = false;
    while (!terminate) {
        if (state.pc < 0 || state.pc >= MEMORYSIZE) {
            abortWithMsg("PC at Invalid Memory Address");
        }

        uint32_t inst = state.mem[state.pc];
        uint8_t opcode = OPCODE(inst);
        uint8_t regA = REG_A(inst);
        uint8_t regB = REG_B(inst);
        uint8_t regDest = REG_DEST(inst);
        int16_t offset = OFF_SET(inst);

        if (opcode < 0 || opcode > 7) {
            abortWithMsg("Invalid OPCODE");
        }
        if (regA > 7 || regA < 0 || regB > 7 || regB < 0 || regDest > 7 || regDest < 0) {
            abortWithMsg("Invalid Register Index");
        }

        printState(&state);

        int next_pc = state.pc + 1;

        switch (opcode) {
            case 0:
                state.reg[regDest] = state.reg[regA] + state.reg[regB];
                break;
            case 1:
                state.reg[regDest] = ~(state.reg[regA] | state.reg[regB]);
                break;
            case 2:
                state.reg[regB] = state.mem[state.reg[regA] + offset];
                break;
            case 3:
                state.mem[state.reg[regA] + offset] = state.reg[regB];
                break;
            case 4:
                if (state.reg[regA] == state.reg[regB]) next_pc = state.pc + 1 + offset;
                break;
            case 5:
                state.reg[regB] = state.pc + 1;
                next_pc = state.reg[regA];
                break;
            case 6:
                terminate = true;
                break;
            case 7:
                break;
            default:
                abortWithMsg("Reached Unreachable OPCODE");
                break;
        }

        state.numInstructionsExecuted++;
        state.pc = next_pc;
    }
    printStats(&state);
    printState(&state);
    convertNum(0);
    //Your code ends here!

    fclose(filePtr);
    return(0);
}

/*
* DO NOT MODIFY ANY OF THE CODE BELOW.
*/

void printState(stateType *statePtr) {
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] 0x%08X\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
	  for (i=0; i<NUMREGS; i++) {
	      printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	  }
    printf("end state\n");
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num)
{
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

/*
 * print end of run statistics like in the spec. **This is not required**,
 * but is helpful in debugging.
 * This should be called once a halt is reached.
 * DO NOT delete this function, or else it won't compile.
 * DO NOT print "@@@" or "end state" in this function
 */
void printStats(stateType *statePtr)
{
    printf("machine halted\n");
    printf("total of %d instructions executed\n", statePtr->numInstructionsExecuted);
    printf("final state of machine:\n");
}

// Returns 1 if string ends with substr, 0 otherwise
static int endsWith(char *string, char *substr)
{
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

/*
* Write any helper functions that you wish down here.
*/
