        lw      0       1       v10
        add     1       1       2
        add     2       2       3
        nor     3       3       4
        lw      0       5       adr
        sw      5       4       0
        lw      5       6       0
        beq     4       6       c1
        halt
c1      add     3       4       7
        sw      0       7       25
        lw      0       1       25
        beq     1       7       ok
        halt
ok      halt
v10     .fill   10
adr     .fill   20