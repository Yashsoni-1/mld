# C++ Memory Leak Detector
## Overview:
In this project I have created a _C++_ memory leak detector tool. This tool keeps track of all the heap objects that the program has created and also how they hold reference to one another. This tool will tell you all the leaked objects, if any. 
## But Why?
Since the advent of _C/C++_, memory management is one of the main responsibility that the programmer has to deal with. C/C++ programs often suffer from two memory related problems
which are _Memory Corruption_ and _Memory Leaks_. Unlike _Java_, _C/C++_ doesn't have the luxury of garbage collection. Since other programming languages does not allow programmer to access the memory directly, they do not suffer memory corruption.

- MLD must know the all the structures being used by application.
- It is the responsibility of the application to tell the MLD library during initialization about all the structures it is using.
- MLD library will maintain the structure database(linked list) to store application structure info.
