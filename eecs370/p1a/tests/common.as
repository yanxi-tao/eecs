        add     0       1       2       Test add reg 0 1 2
        nor     3       4       5       Test nor reg 3 4 5
        lw      6       7       0       Test lw reg 6 7 offset 0
        sw      0       1       -1      Test sw negative numeric offset
        beq     2       3       1       Test beq numeric offset
        jalr    4       5               Test jalr
        halt                            Test halt
        noop                            Test noop
        .fill   2147483647              Max positive integer
        .fill   -2147483648             Min negative integer