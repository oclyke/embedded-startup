This example demonstrates how static/global object constructors are called. Usually this would occur before ```main``` but in this example it is done explicitly.

``` c
// https://bitbashing.io/embedded-cpp.html
static void callConstructors()
{
  // Start and end points of the constructor list,
  // defined by the linker script.
  extern void (*__init_array_start)();
  extern void (*__init_array_end)();

  // Call each function in the list.
  // We have to take the address of the symbols, as __init_array_start *is*
  // the first function pointer, not the address of it.
  for (void (**p)() = &__init_array_start; p < &__init_array_end; ++p) {
      (*p)();
  }
}
```

The process used here raises a few good questsions:

* How do constructors get placed into the init array?
    * GCC magic. Just kidding. GCC recognizes calls to these objects and generates the necessary code, then tags them with ```init_array``` so that the linker can group them together.
* Why can you call all the constructors with jsut one function signature?
    * Probably because they are all global/static so the arguments to the constructor are known at compile time and can be rolled into a preamble of the function call
* How do the created objects get put into memory at the correct locations?
    * Again, this is likely thanks to the code that GCC generates with a special preamble

## Key Info
