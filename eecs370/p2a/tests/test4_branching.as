Loop    add     1   2   3
        beq     0   0   Target
        noop
        noop
Target  add     1   1   1
        add     1   1   1
        beq     0   0   Loop
        beq     0   0   local
        halt
local   .fill   0