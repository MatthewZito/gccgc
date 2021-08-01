# gccgc

**implementation abstract**

automatic garbage collection & memory leak detection for the C programming language

What would it take to have automatic garbage collection for heap memory in C?

This is a small project intended to *explore* this problem; it is not intended to be consumed by production applications, nor is it intended to be an ideal solution to said problem.

`gccgc` implements a depth first search using a linked list that traverses C structures, no matter how nested, and records their properties in a directed cyclic graph. This data structure tracks memory that has been allocated on the heap and reports any memory leaks which have occurred.

We begin traversal at the top-most, or root object of a structure. This object is often a global object for which memory leaks cannot be detected given the detection algorithm begins after the root object.
