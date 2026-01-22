        lw      0       1       mcand       ; Load factor 1 to reg1
        lw      0       2       mplier      ; Load factor 2 to reg2
        add     0       0       3           ; CLEAR reg3 (Accumulator/Product) to 0
        lw      0       7       digit       ; Load current cnt (mask) to reg7 (start at 1)
        lw      0       6       max         ; Load max cnt to reg6 (stop limit)
loop    beq     6       7       exit        ; If mask == max, exit
        nor     7       0       5           ; reg5 = NOT reg7 (Using reg0 as 0)
        nor     2       0       4           ; reg4 = NOT reg2 (Using temporary reg4)
        nor     4       5       5           ; reg5 = NOR(NOT reg2, NOT reg7) = AND(reg2, reg7)
        beq     0       5       noadd       ; If AND result is 0, skip the addition
        add     3       1       3           ; Result += Multiplicand
noadd   add     7       7       7           ; Shift mask left (1 -> 2 -> 4...)
        add     1       1       1           ; Shift Multiplicand left
        beq     0       0       loop        ; Unconditional jump to start
exit    halt
mcand   .fill   6203
mplier  .fill   1429
digit   .fill   1
max     .fill   32768                       ; This will run for 15 bits, assumed fro spec