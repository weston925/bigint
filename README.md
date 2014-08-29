bigint
======

A C++ implementation of a big integer library


All of the code here is written by me (Weston Witt). You can contact me through my email address weston925@gmail.com. It requires C++11 or better to be used, and I wrote it using Visual Studio Professional 2013 so I don't know if it will work with any previous versions of Visual Studio or any other C++ compilers. You will have to find that out yourself.

I added comments to try and explain how it works throughout all of the code except for the BigIntegerUtil files. I wrote that code a long time ago and it will take me a while to figure out my logic on it again and explain it through comments. But those are just some helper functions and overloads to work with std::cout and they most likely need a ton of optimization... (but at least it works right? :D) If anything of my code doesn't make sense or you can't figure out why it's there, feel free to send me an email and ask me.


I did do some testing to make sure it works, but not enough to be 100% sure that it's bug-free. So please download this code and test it out yourself and tell me if something went wrong. I'll do my best to fix the code as quickly as possible. And any suggestions would be greatly appreciated!


Although all of the code in here is initially written by me, I have to give some credit to Matt McCutchen and his bigint library (http://mattmccutchen.net/bigint/). I liked that he had both a signed and unsigned version and I used a lot of the names he used for both the classes and functions. (What can I say? They made sense :D) But my code and how all the internals works has very little in common with his code. The following are a few things I did different:

1. I used the standard container deque instead of creating my own. Deque allocates blocks of data of a certain size with the information to get to the next block instead of allocating all the data in a single array and having to reallocate when a bigger array is needed (very similar to a vector).
2. I implimented the bitwise NOT operator (~). I still don't know why he excluded that one.
3. I used a templated conversion operator and a templated assignment operator to convert to and from primitive data types instead of named functions. Which also allows the use of those data types in other operators. (Note: the templates are limited to integral data types using the function std::is_integral\<T\>::value)
4. The increment (++) and decrement (--) operators act exactly like they do on normal data types.
5. I used the following techniques to both increase the speed and reduce memory usage:
  * Implemented move semantics to increase speed and slightly reduce memory usage
  * Used a shared_ptr to reduce memory usage and increase speed in certain situations
  * Wait until something is changed to create the memory pointed by shared_ptr to increase speed and reduce memory in certain situations (the default value is zero when it's not created)

And many more that aren't mentioned here.
