#pragma once
#include <cstdlib>
#include <utility>

class ControlBlock {
public:
    ControlBlock() noexcept : use_count(1), weak_use_count(1) {
    }
    virtual ~ControlBlock() {
    }

    virtual void DelObject() {
    }
    virtual void DelThis() {
    }
    void IncRef() {
        if (++use_count == 1)
            IncWeakRef();
    }
    void DecRef() {
        if (--use_count == 0) {
            DelObject();
            DecWeakRef();
        }
    }
    const size_t& UseCount() {
        return use_count;
    }
    void IncWeakRef() {
        ++weak_use_count;
    }
    void DecWeakRef() {
        if (--weak_use_count == 0) {
            DelThis();
        }
    }

private:
    size_t use_count;
    size_t weak_use_count;
};

template <typename T>
class ControlBlockPointerImp : public ControlBlock {
public:
    ControlBlockPointerImp(T* ptr) noexcept : ControlBlock(), object(ptr) {
    }
    ~ControlBlockPointerImp() = default;

    void DelObject() {
        if (object)
            delete object;

        object = nullptr;
    }

    void DelThis() {
        delete this;
    }

private:
    T* object;
};

template <typename T>
class ControlBlockObjectImp : public ControlBlock {
public:
    template <typename... Args>
    ControlBlockObjectImp(Args&&... args) : ControlBlock(), object(std::forward<Args>(args)...) {
    }
    ~ControlBlockObjectImp() = default;

    bool StoreObject() {
        return true;
    }
    T* GetObject() {
        return &object;
    }

    void DelObject() {
        object.~T();
    }

    void DelThis() {
        ::operator delete(this);
    }

private:
    T object;
};