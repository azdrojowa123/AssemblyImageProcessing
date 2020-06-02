.global times

.text

times:
push %ebx

xor %eax, %eax
cpuid 
rdtsc

pop %ebx

ret 
