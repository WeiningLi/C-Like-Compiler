lis $4
.word 4
lis $8
.word 0
lis $3	; height
.word 1
lis $11
.word 1
lis $6	; init array len
.word 4
sub $28, $0, $3	; -1 const
lis $27
.word 3
lis $17
.word 2

sw $0, -4($30)
bne $2, $27, rec	; only 1 node
jr $31

rec:
add $7, $0, $0
beq $7, $6, 3
sw $31, 4000($1)
lis $23
.word loop
jalr $23
lw $31, 4000($1)
bne $6, $0, 1
jr $31
add $8, $0, $0
add $3, $3, $11	; update height 
beq $0, $0, rec

loop:
lw $21, -4($30)
sub $30, $30, $4
add $21, $21, $1
lw $22, 4($21)
beq $22, $28, 7	; child DNE
mult $22, $4
mflo $22
sub $23, $6, $7
sub $23, $30, $23
sub $23, $23, $8
sw $22, 0($23)
add $8, $4, $8

lw $22, 8($21)
beq $22, $28, 7 ; child DNE
mult $22, $4
mflo $22
sub $23, $6, $7
sub $23, $30, $23
sub $23, $23, $8
sw $22, 0($23)
add $8, $4, $8

add $7, $7, $4
bne $7, $6, loop 
add $6, $8, $0
jr $31
