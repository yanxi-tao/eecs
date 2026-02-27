        add     0   1   2
        nor     3   4   5
        lw      0   1   0
        sw      6   7   -1
        jalr    5   2
        beq     0   1   label
        noop
        halt
label   .fill   10