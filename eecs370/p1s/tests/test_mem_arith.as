        lw      0       1       b10
        lw      0       2       bm5
        lw      0       3       val
        sw      1       3       -10
        sw      2       3       15
        lw      1       4       0
        lw      2       5       15
        beq     4       5       c1
        halt
c1      lw      0       6       0
        beq     6       3       c2
        halt
c2      sw      0       3       20
        lw      0       7       20
        beq     7       3       ok
        halt
ok      halt
b10     .fill   10
bm5     .fill   -5
val     .fill   999