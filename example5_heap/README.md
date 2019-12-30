The heap is a memory space that is dedicated to dynamically allocated persitent data. 

The ```_sbrk``` function is our platform-specific helper to ```malloc``` which tells malloc if there is enough free memory to satisfy the request

``` c
/* Excellent insight from Martin T. https://embdev.net/topic/linker-error-undefined-reference-to-_sbrk */
extern "C" caddr_t _sbrk ( int incr ){
    unsigned char* prev_heap;
    unsigned char  local;

    // Initialize the heap pointer
    if (_eheap == NULL) {
        _eheap = (unsigned char*)&_sheap;
    }
    prev_heap = _eheap;

    // Check that the requested amount of memory is available
    if( ((&local)-(_eheap)) >= (incr + MEMORY_HEADSPACE) ){
        _eheap += incr;
    }else{
        // set errno to ENOMEM
        prev_heap = (unsigned char*)-1;
    }

    // Return the previous heap (or the error code)
    return (caddr_t) prev_heap;
}
```

```malloc``` takes care of the rest of the dynamic memory implementation including provision of information that allows ```free``` to release allocated memory.

## Key Info
