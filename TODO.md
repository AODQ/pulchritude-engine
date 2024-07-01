- PuleStringView could just be a PuleString with a null allocator
  - this could help when we don't really care who allocates the string
  - however one issue is with PuleStringView you know at compile-time the
      string doesn't have an allocator
  - ultimately, PSV is useful for C ABI interop and worth the extra symbol
  - so then a PuleString that CAN act as a view is what will be most useful
     in those cases the allocator is null. But what would this function
     be called since puleStringView is already for PSV


- need to split out binding generator that generates a file for all
    the toStr functions
