start   lw      0       1       five    Load address of 'five' (should be 5)
        beq     0       0       five    Branch to 'five' (offset should be 5 - (1+1) = 3)
        beq     0       0       start   Branch backward (offset should be 0 - (2+1) = -3)
        noop
        halt
five    .fill   5