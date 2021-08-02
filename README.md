# gccgc

automatic garbage collection & memory leak detection for the C programming language

What would it take to have automatic garbage collection for heap memory in C?

This is a small project intended to *explore* this notion; it is not intended to be consumed by production applications, nor is it intended to be an ideal solution.

## Memory Leak Detection

First, we initialize a registrar in which all structures and aggregations thereof are recorded; this registrar includes metadata about each structure, such as its members, their respective sizes, and their members if applicable. The user must register dynamic objects using the `gccgc` API.

The registrar is comprised of nodes in a linked list, which are then arranged by `gccgc` into a directed cyclic graph. Each node contains flags utilized by a depth first search algorithm to inform the traverser whether the current node has been traversed, and whether the node constitutes as a root object. Traversal is implemented using the visitor pattern.

We begin traversal at the top-most, or root object of a structure. This object is often a global object for which memory leaks cannot be detected given the detection algorithm begins at the root object.

After traversing all objects, we recurse and visit their child objects. When running a memory leak report, any object that has been entered into the registrar but not marked as visited is an unreachable object such as a dangling pointer and is therefore a memory leak.
