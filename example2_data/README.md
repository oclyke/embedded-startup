Now we are exploring the function of the data section copy loop
The compiler takes variables that are initialized with a unique value and
puts them in a section called '.data'. That section gets space reserved in
both flash memory and ram since the ram is volatile but the data needs to 
start with the same value each time.

During startup the copy loop copies the unique values from the flash memory
addresses to the ram addresses. The assembly looks like this:

```assembly
    ldr     r0, =_init_data
    ldr     r1, =_sdata
    ldr     r2, =_edata
copy_loop:
        cmp   r1, r2
        beq   copy_end
        ldr   r3, [r0], #4
        str   r3, [r1], #4
        b     copy_loop
copy_end:
);

```




## Key Info

Startup Metrics:

Number of unique bytes | Number of Cycles in Copy Loop
--- | ---
0 | 5
1 | 10
2 | 10
3 | 10
4 | 10
5 | 15
6 | 15
7 | 15
8 | 15
