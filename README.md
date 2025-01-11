# smart_ptrs
This is my incomplete implementation of C++ smart pointers. It implements main part smart pointers (§20.7) of ISO C++ 2011 with their features like make_shared, enable_shared_from_this, support of custom deleter for unique_ptr and other. However, custom allocator and custom deleter for shared_ptr and some small details are not supported.

Smart pointers are a distinctive feature of modern C++. They enable automatic object lifetime management. Implementing smart pointers is a great opportunity to deepen my understanding of smart pointers themselves and dynamic memory management in gerenal. The implementation also involves an understanding of templates and move semantics.
# unique_ptr
unique_ptr is a smart pointer that ensures exclusive ownership of a dynamically allocated object. When the unique_ptr goes out of scope, the resource it manages is automatically deallocated, preventing memory leaks.
## Empty base optimization(EBO) to store stateless deleter with zero size fee
unique_ptr supports custom deleters, which can either be:
* Stateless deleters: Functions or functors without any state.
* Stateful deleters: Functors or callable objects that carry data (e.g., a lambda with captured variables).

unique_ptr must store deleter as a member. If this member is an empty object (e.g., a stateless deleter), EBO allows the unique_ptr to avoid wasting memory by storing the empty object without increasing its size.

In C++, an empty class has no data members and occupies a minimal amount of memory (typically 1 byte for alignment purposes). When a class inherits from an empty base class, the compiler can apply EBO to eliminate the space occupied by the empty base, as long as no ambiguity arises in the object layout.

"compressed_pair.h" contains my implementation of a compressed pair that supports EBO. I used this to implement a unique_ptr.

# shared_ptr and weak_ptr
shared_ptr is a smart pointer that allows multiple shared_ptr instances to share ownership of a dynamically allocated resource. When the last shared_ptr owning a resource is destroyed or reset, the resource is automatically deallocated.

weak_ptr is a non-owning smart pointer that observes a resource managed by shared_ptr, breaking ownership cycles and enabling safe access to resources without extending their lifetime.


shared_ptr and weak_ptr do not directly manage the object. They do it indirectly through the control block. The control block will stay alive until no associated shared_ptr/weak_ptr lives.

Control block manages:

* The reference count (number of shared_ptr instances owning the resource).
* The weak count (number of weak_ptr instances observing the resource).
* The pointer to object or the object itself.

### Optimizations
There are two control block realization in my code. First one with pointer to the object, second one with object as a field. A control block with an object inside is created only when the user calls make_shared. This allows only one expensive memory allocation operation to be used to create a control block and an object.

When use_count == 0 but weak_count != 0, the resource (managed object) is deleted, but the control block itself is retained.
In this way, when we try to use a weak_ptr which points to an already destroyed object, we can learn from the control block that this weak_ptr has been expired and the object no longer exists.



