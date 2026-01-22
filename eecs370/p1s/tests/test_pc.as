        lw      0       1       a4
        jalr    1       2
        lw      0       6       two
        beq     2       6       err
tgt     lw      0       3       a7
        jalr    3       4
        halt
        lw      0       5       five
        beq     4       5       ok
        halt
ok      halt
err     halt
a4      .fill   4
a7      .fill   7
two     .fill   2
five    .fill   5