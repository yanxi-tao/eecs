        lw      0       1       n
        lw      0       2       r
        lw      0       4       Faddr       load function address
        jalr    4       7                   call function
        halt
faddr   beq     2       0       base        if (r == 0) return 1
        beq     1       2       base        if (n == r) return 1
        lw      0       6       one
        sw      5       7       Stack       push RA
        add     5       6       5           sp++
        sw      5       1       Stack       push n
        add     5       6       5           sp++
        sw      5       2       Stack       push r
        add     5       6       5           sp++
        lw      0       6       neg1
        add     1       6       1           n = n - 1
        lw      0       4       Faddr
        jalr    4       7                   result is in r3
        lw      0       6       neg1
        add     5       6       5           sp--
        lw      5       2       Stack       pop original r into r2
        add     5       6       5           sp--
        lw      5       1       Stack       pop original n into r1
        sw      5       3       Stack       push first result (r3) onto stack where 'n' used to be!
        lw      0       6       one
        add     5       6       5           sp++ (stack = [RA, res1])
        lw      0       6       neg1
        add     1       6       1           n = n - 1
        add     2       6       2           r = r - 1
        lw      0       4       Faddr
        jalr    4       7                   result is in r3
        lw      0       6       neg1
        add     5       6       5           sp--
        lw      5       1       Stack       pop first result into r1
        add     3       1       3           r3 = res1 + res2
        add     5       6       5           sp--
        lw      5       7       Stack       pop RA
        jalr    7       4                   return to caller
base    lw      0       3       one         set return value to 1
        jalr    7       4                   return to caller
n       .fill   7
r       .fill   3
one     .fill   1
neg1    .fill   -1
Faddr   .fill   faddr