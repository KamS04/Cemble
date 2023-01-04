log r1
log &r1
log &01
mov8 $C0DE, &B00B
mov8 &B00B, r1
movl r1, &B00B
movh r1, &B00B
mov8 &r2, r3
mov8 r3, &r4
mov $B00B, r2
mov r1, r2
mov r1, &r2
mov r1, &B00B
mov &C0DE, r2
mov $C0DE, &B00B
mov &r1, r2
mov $02, &r2, r3
int $01
rti
add r1, r2
add $02, r2
sub $02, r2
sub r2, $02
sub r2, r1
inc r1
dec r2
mul $02, r1
mul r1, r2
lsf r1, $04
lsf r1, r2
rsf r1, $ff
rsf r1, r2
and r1, $ff
and r1, r2
or r1, $f00F
or r1, r2
xor r1, $ff
xor r1, r2
not r1
jne $02, &C0DE
jne r1, &C0DE
jeq r1, &C0DE
jeq $02, &C0DE
jlt r1, &C0DE
jlt $02, &B00B
jgt r1, &B00B
jgt $02, &C0DE
jle r1, &C0DE
jle $02, &B00B
jge r1, &B00B
jge $02, &C0DE
psh $02
psh r1
pop r1
cal $C0DE
cal r1
ret
hlt
brk