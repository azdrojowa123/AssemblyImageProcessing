.global filter

.section .data 

MatrixK_1: .byte -1, -1, 0, 0
MatrixK_2: .byte -1, 0, 1, 0
MatrixK_3: .byte 0, 1, 1, 0
height: .space 4
width: .space 4
temp: .space 4
width1: .space 4
addNormalization: .word 765, 765, 0, 0 

.section .text

.macro Timer
xor %eax, %eax
cpuid 
rdtsc
.endm


filter:
pushl %ebp
mov %esp, %ebp
push %ebx
push %esi
push %edi

 
mov 8(%ebp), %edx # wskaznik na M
mov 12(%ebp), %edi #wskaznik na W
mov 16(%ebp), %eax #width
mov 20(%ebp), %esi #height 
 
decl %esi
decl %esi
movl %esi, height
movl %eax, width
decl %eax
decl %eax
movl %eax, width1

movd addNormalization, %mm3
movd MatrixK_1, %mm4
movd MatrixK_2, %mm5
movd MatrixK_3, %mm6

movl $-1, %ebx

FirstLoop: #petla od wysokości

movl $0, %esi #iterator po width 
incl %ebx #iterator po height
cmpl height, %ebx
jge End
 
SecondLoop: #petla od szerokosci

mov %ebx, %ecx
imull width, %ecx #mnożenie h*width 
add %esi, %ecx #w ecx mam h*width +w, grejt 

pinsrw $0, 0(%edx, %ecx,1), %mm0
pinsrw $1, 2(%edx, %ecx,1), %mm0 #pierwszy rząd mcierzy m w wektorze

pmaddubsw %mm4, %mm0

add width, %ecx #(h+1)*width= h*width +width
mov %ecx, temp

pinsrw $0, 0(%edx, %ecx,1), %mm1
pinsrw $1, 2(%edx, %ecx,1), %mm1 #drugi rząd mcierzy m w wektorze

pmaddubsw %mm5, %mm1

add width, %ecx #(h+2)*width= h*width +2*width

pinsrw $0, 0(%edx, %ecx,1), %mm2
pinsrw $1, 2(%edx, %ecx,1), %mm2 #trzeci rząd mcierzy m w wektorze

pmaddubsw %mm6, %mm2

paddw %mm1, %mm0
paddw %mm2, %mm0
phaddw %mm0, %mm0 #wyniczek zachowany w mm0 
paddw %mm3, %mm0

psrlw $3, %mm0


pextrw $0, %mm0, %eax
mov temp, %ecx
movb %al, 1(%edi,%ecx,1)

pxor %mm0, %mm0
pxor %mm1, %mm1
pxor %mm2, %mm2

incl %esi 
cmpl width1, %esi
jl SecondLoop  


jmp FirstLoop

End:
emms
pop %edi
pop %esi
pop %ebx
pop %ebp
ret
 

