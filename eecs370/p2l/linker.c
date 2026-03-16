/**
 * Project 2
 * LC-2K Linker
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSIZE 500
#define MAXLINELENGTH 1000
#define MAXFILES 6

static inline void printHexToFile(FILE *, int);
static int endsWith(char *, char *);

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
    char label[7];
    char location;
    unsigned int offset;
};

struct RelocationTableEntry {
    unsigned int file;
    unsigned int offset;
    char inst[6];
    char label[7];
};

struct FileData {
    unsigned int textSize;
    unsigned int dataSize;
    unsigned int symbolTableSize;
    unsigned int relocationTableSize;
    unsigned int textStartingLine; // in final executable
    unsigned int dataStartingLine; // in final executable
    int text[MAXSIZE];
    int data[MAXSIZE];
    SymbolTableEntry symbolTable[MAXSIZE];
    RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
    unsigned int textSize;
    unsigned int dataSize;
    unsigned int symbolTableSize;
    unsigned int relocationTableSize;
    int text[MAXSIZE * MAXFILES];
    int data[MAXSIZE * MAXFILES];
    SymbolTableEntry symbolTable[MAXSIZE * MAXFILES];
    RelocationTableEntry relocTable[MAXSIZE * MAXFILES];
};

// Helper
void abortWithMsg(const char *msg) {
    fprintf(stderr, "ErrAbort: %s\n", msg);
    exit(1);
}

FileData files[MAXFILES];
CombinedFiles e;

bool isDuplicateSym(const char *label) {
    for (int i = 0; i < e.symbolTableSize; i++) {
        if (strcmp(label, e.symbolTable[i].label) == 0) return true;
    }
    return false;
}

SymbolTableEntry *lookUpSym(const char *label) {
    for (int i = 0; i < e.symbolTableSize; i++) {
        if (strcmp(label, e.symbolTable[i].label) == 0) return &e.symbolTable[i];
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    char *inFileStr, *outFileStr;
    FILE *inFilePtr, *outFilePtr;
    unsigned int i, j;

    if (argc <= 2 || argc > 8 ) {
        printf("error: usage: %s <MAIN-object-file> ... <object-file> ... <machine-code-file>, with at most 5 object files\n",
                argv[0]);
        exit(1);
    }

    outFileStr = argv[argc - 1];
    if (endsWith(outFileStr, ".obj")) {
        printf("error: no machine code file given\n");
        exit(1);
    }

    outFilePtr = fopen(outFileStr, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileStr);
        exit(1);
    }

    // read in all files and combine into a "master" file
    for (i = 0; i < argc - 2; ++i) {
        inFileStr = argv[i+1];
        inFilePtr = fopen(inFileStr, "r");

        if (inFilePtr == NULL) {
            printf("error in opening %s\n", inFileStr);
            exit(1);
        }

        char line[MAXLINELENGTH];
        unsigned int textSize, dataSize, symbolTableSize, relocationTableSize;

        // parse first line of file
        fgets(line, MAXSIZE, inFilePtr);
        sscanf(line, "%d %d %d %d",
                &textSize, &dataSize, &symbolTableSize, &relocationTableSize);

        files[i].textSize = textSize;
        files[i].dataSize = dataSize;
        files[i].symbolTableSize = symbolTableSize;
        files[i].relocationTableSize = relocationTableSize;

        // read in text section
        int instr;
        for (j = 0; j < textSize; ++j) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            instr = strtol(line, NULL, 0);
            files[i].text[j] = instr;
        }

        // read in data section
        int data;
        for (j = 0; j < dataSize; ++j) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            data = strtol(line, NULL, 0);
            files[i].data[j] = data;
        }

        // read in the symbol table
        char label[7];
        char type;
        unsigned int addr;
        for (j = 0; j < symbolTableSize; ++j) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            sscanf(line, "%s %c %d", label, &type, &addr);
            files[i].symbolTable[j].offset = addr;
            strcpy(files[i].symbolTable[j].label, label);
            files[i].symbolTable[j].location = type;
        }

        // read in relocation table
        char opcode[7];
        for (j = 0; j < relocationTableSize; ++j) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            sscanf(line, "%d %s %s", &addr, opcode, label);
            files[i].relocTable[j].offset = addr;
            strcpy(files[i].relocTable[j].inst, opcode);
            strcpy(files[i].relocTable[j].label, label);
            files[i].relocTable[j].file = i;
        }
        fclose(inFilePtr);
    } // end reading files

    // *** LINKING PROCESS START ***

    // Compute text/data starting line by section in final exec
    e.textSize = 0;
    e.dataSize = 0;
    for (i = 0; i < argc - 2; ++i) {
        files[i].textStartingLine = e.textSize;
        e.textSize += files[i].textSize;
    }
    for (i = 0; i < argc - 2; ++i) {
        files[i].dataStartingLine = e.dataSize;
        e.dataSize += files[i].dataSize;
    }

    // Construct global symTable + populate combined text/data
    e.symbolTableSize = 0;
    for (i = 0; i < argc - 2; ++i) {
        // concat text
        for (j = 0; j < files[i].textSize; ++j) {
            e.text[files[i].textStartingLine + j] = files[i].text[j];
        }
        // concat data
        for (j = 0; j < files[i].dataSize; ++j) {
            e.data[files[i].dataStartingLine + j] = files[i].data[j];
        }

        // g-symtTable
        for (j = 0; j < files[i].symbolTableSize; ++j) {
            SymbolTableEntry *sym = &files[i].symbolTable[j];

            if (sym->location == 'U') continue;

            if (isDuplicateSym(sym->label)) abortWithMsg("Duplicate defined global labels");
            if (strcmp(sym->label, "Stack") == 0) abortWithMsg("Stack label defined by an object file");

            strcpy(e.symbolTable[e.symbolTableSize].label, sym->label);
            e.symbolTable[e.symbolTableSize].location = sym->location;

            if (sym->location == 'T') {
                e.symbolTable[e.symbolTableSize].offset = sym->offset + files[i].textStartingLine;
            } else {
                e.symbolTable[e.symbolTableSize].offset = sym->offset + e.textSize + files[i].dataStartingLine;
            }
            ++e.symbolTableSize;
        }
    }

    // patch by relocTable
    for (i = 0; i < argc - 2; ++i) {
        for (j = 0; j < files[i].relocationTableSize; ++j) {
            RelocationTableEntry *reloc = &files[i].relocTable[j];
            char *label = reloc->label;

            int is_local = (label[0] >= 'a' && label[0] <= 'z');
            int is_stack = (strcmp(label, "Stack") == 0);
            int is_text_inst = (strcmp(reloc->inst, ".fill") != 0);

            int section_offset = reloc->offset;
            int *instr_ptr;

            if (is_text_inst) {
                instr_ptr = &e.text[files[i].textStartingLine + section_offset];
            } else {
                instr_ptr = &e.data[files[i].dataStartingLine + section_offset];
            }

            int sym_addr = 0;

            if (is_local) {
                int old_addr = is_text_inst ? ((*instr_ptr) & 0xFFFF) : *instr_ptr;

                if (old_addr < files[i].textSize) {
                    // originally n text
                    sym_addr = old_addr + files[i].textStartingLine;
                } else {
                    // originally in data
                    sym_addr = old_addr - files[i].textSize + e.textSize + files[i].dataStartingLine;
                }
            } else if (is_stack) {
                // yo stackyyyyy u got special treatment
                sym_addr = e.textSize + e.dataSize;
            } else {
                // globals
                SymbolTableEntry *sym = lookUpSym(label);
                if (!sym) abortWithMsg("Undefined global labels");
                sym_addr = sym->offset;
            }

            // store resolved
            if (is_text_inst) {
                *instr_ptr = (*instr_ptr & 0xFFFF0000) | (sym_addr & 0xFFFF);
            } else {
                *instr_ptr = sym_addr;
            }
        }
    }

    // write-back
    for (i = 0; i < e.textSize; i++) {
        printHexToFile(outFilePtr, e.text[i]);
    }
    for (i = 0; i < e.dataSize; i++) {
        printHexToFile(outFilePtr, e.data[i]);
    }

    fclose(outFilePtr);
} // main

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