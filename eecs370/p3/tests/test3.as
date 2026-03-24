        lw      0       1       one
        beq     0       0       skip    // Always branches
        add     1       1       2       // SQUASH 1
        add     1       1       3       // SQUASH 2
        add     1       1       4       // SQUASH 3
skip    halt
one     .fill   1
