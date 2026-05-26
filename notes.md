- [ ] Parse chunked requests -> "Transfer-Encoding"
- [ ] maybe remove the null terminated strings, make a type String 
    Idk about this, cstrings are necessary for various functions
- [ ] Functions generally should not malloc memory and then return the allocated memory.
    We should pass a preallocated buffer/memory to the function that needs it.
    This way the caller allocate and frees memory, its a clear ownership.
    Example:
    ```
        // caller allocates
        char hash[BCRYPT_HASHSIZE];
        get_user_hash(username, hash, BCRYPT_HASHSIZE);
        
        // vs function allocates
        char *hash = get_user_hash(username); // caller must remember to free
    ```
- [ ] Implement closing TCP by inactive time
- [ ] Close on "Connection: close"
- [ ] for safety change strlens for strnlens (it sets max len)
    This can be done just for client requests. 
    I trust strlen of internal server things
- [ ] Make endpoints for all joke db CRUD
- [ ] Make someway of passing request params in HttpRequest struct
- [ ] For now we have fixed size of route params (max 8), 
    think of another way
- [ ] This route match struct is strange and i dislike it. 
- [ ] Maybe I should get all tokens between '/' in paths. 
    Then just after I can validate and extract only the parameters 
- [ ] create a dynamic array module, use on get_all_jokes please
## Archive


- [x] need to implement resizable buffer for reading socket.
Better performance on large requests
- [x] all snake case except types
