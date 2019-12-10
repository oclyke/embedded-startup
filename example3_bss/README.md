Now we are exploring the function of the bss section fill loop
The compiler takes variables that are trivially initialized (0) and puts 
them in a section called '.bss'. That section gets space reserved only in
ram. Since the value of that region is all 0 the value 0 is stored in code
(text section) where a loop fills the bss section.

During startup the bss loop fills the ram addresses with assembly that looks like this:

```assembly
    ldr     r0, =_sbss
    ldr     r1, =_ebss
    mov     r2, #0
zero_loop:
        cmp     r0, r1
        it      lt
        strlt   r2, [r0], #4
        blt     zero_loop
);

```




## Key Info

Startup Metrics:

Number of bytes | Number of Cycles in BSS Fill Loop
--- | ---
0 | 7
1 | 11
2 | 11
3 | 11
4 | 11
5 | 15
6 | 15
7 | 15
8 | 15
9 | 19

