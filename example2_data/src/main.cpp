/* example2_data */

// Choose some number of bytes to copy from flash to ram
#define BYTES_TO_COPY 8

// Automatially add some variables that won't be optimized
// away to fill up the number of requested bytes
#ifdef BYTES_TO_COPY
#if BYTES_TO_COPY > 0
static volatile char var11 = 0xDE;
#endif
#if BYTES_TO_COPY > 1
static volatile char var12 = 0xAD;
#endif
#if BYTES_TO_COPY > 2
static volatile char var13 = 0xBE;
#endif
#if BYTES_TO_COPY > 3
static volatile char var14 = 0xEF;
#endif

#if BYTES_TO_COPY > 4
static volatile char var21 = 0xC0;
#endif
#if BYTES_TO_COPY > 5
static volatile char var22 = 0xFE;
#endif
#if BYTES_TO_COPY > 6
static volatile char var23 = 0xE6;
#endif
#if BYTES_TO_COPY > 7
static volatile char var24 = 0x0D;
#endif
#endif // BYTES_TO_COPY


int main()
{
    while(1){
#ifdef BYTES_TO_COPY
#if BYTES_TO_COPY > 0
		var11++;
#endif
#if BYTES_TO_COPY > 1
		var12++;
#endif
#if BYTES_TO_COPY > 2
		var13++;
#endif
#if BYTES_TO_COPY > 3
		var14++;
#endif

#if BYTES_TO_COPY > 4
		var21++;
#endif
#if BYTES_TO_COPY > 5
		var22++;
#endif
#if BYTES_TO_COPY > 6
		var23++;
#endif
#if BYTES_TO_COPY > 7
		var24++;
#endif
#endif // BYTES_TO_COPY
    } // while(1)
}
