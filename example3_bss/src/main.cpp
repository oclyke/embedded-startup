/* example3_bss */

// Choose some number of bytes to pre-fill with zeroes in ram
#define BYTES_TO_FILL 1


// Create an uninitialized array 
// (trivial - all elements are 0 - this is default when no initializer is set)
#ifdef 	BYTES_TO_FILL
#if 		BYTES_TO_FILL > 0
static volatile char varARR[BYTES_TO_FILL] = {0};
#endif
#endif // BYTES_TO_FILL


int main()
{
    while(1){
#ifdef 	BYTES_TO_FILL
#if 		BYTES_TO_FILL > 0
			varARR[0]++;
#endif
#endif // BYTES_TO_FILL
    } // while(1)
}
