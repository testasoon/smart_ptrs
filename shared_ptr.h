#pragma once

#include <cstddef> // std::nullptr_t
#include "shared_weak_fwd.h"
#include "control_block.h"
#include "bad_weak_ptr.h"

template <typename T>
class SharedPtr {
public:
    SharedPtr() noexcept : block(nullptr), obj(nullptr) {
    }
    SharedPtr(std::nullptr_t) noexcept : block(nullptr), obj(nullptr) {
    }

    template <typename U>
    SharedPtr(U* ptr) noexcept
        : block(static_cast<ControlBlock*>(new ControlBlockPointerImp<U>(ptr))),
          obj(static_cast<T*>(ptr)) {
        if constexpr (std::is_convertible_v<U*, EnableSharedFromThisBase*>) {
            InitWeakThis(ptr);
        }
    }

    explicit SharedPtr(T* ptr) noexcept
        : block(static_cast<ControlBlock*>(new ControlBlockPointerImp<T>(ptr))), obj(ptr) {
        if constexpr (std::is_convertible_v<T*, EnableSharedFromThisBase*>) {
            InitWeakThis(ptr);
        }
    }

    template <typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
    SharedPtr(const SharedPtr<U>& other) noexcept
        : block(other.block), obj(static_cast<T*>(other.Get())) {
        if (block)
            block->IncRef();
    }
    SharedPtr(const SharedPtr& other) noexcept : block(other.block), obj(other.Get()) {
        if (block)
            block->IncRef();
    }

    template <typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
    SharedPtr(SharedPtr<U>&& other) noexcept
        : block(other.block), obj(static_cast<T*>(other.Get())) {
        other.block = nullptr;
        other.obj = nullptr;
    }
    SharedPtr(SharedPtr&& other) noexcept : block(other.block), obj(other.Get()) {
        other.block = nullptr;
        other.obj = nullptr;
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other, T* ptr) noexcept : block(other.block), obj(ptr) {
        if (block)
            block->IncRef();
    }

    SharedPtr(const WeakPtr<T>& other) {
        if (other.Expired())
            throw BadWeakPtr();
        block = other.block;
        obj = other.obj;
        if (block)
            block->IncRef();
    }

    SharedPtr(const WeakPtr<T>& other, std::nothrow_t) noexcept
        : block(other.block), obj(other.obj) {
        if (other.Expired()) {
            obj = nullptr;
        }
        if (block)
            block->IncRef();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }

        if (block)
            block->DecRef();

        block = other.block;
        obj = other.obj;
        if (block)
            block->IncRef();
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }

        if (block)
            block->DecRef();

        block = other.block;
        obj = other.obj;
        other.obj = nullptr;
        other.block = nullptr;
        return *this;
    }

    ~SharedPtr() {
        if (block)
            block->DecRef();
    }

    void Reset() {
        if (block)
            block->DecRef();

        block = nullptr;
        obj = nullptr;
    }

    template <typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
    void Reset(U* ptr = nullptr) {
        if (block) {
            block->DecRef();

            block = nullptr;
            obj = nullptr;
        }
        if (ptr) {
            block = static_cast<ControlBlock*>(new ControlBlockPointerImp<U>(ptr));
            obj = static_cast<T*>(ptr);
        }
    }

    void Swap(SharedPtr& other) {
        std::swap(block, other.block);
        std::swap(obj, other.obj);
    }

    T* Get() const {
        return obj;
    }
    T& operator*() const {
        return *Get();
    }
    T* operator->() const {
        return Get();
    }
    size_t UseCount() const {
        if (!block)
            return 0;
        return block->UseCount();
    }
    explicit operator bool() const {
        return !(obj == nullptr);
    }

private:
    ControlBlock* block;
    T* obj;

    SharedPtr(ControlBlock* block_, T* object) noexcept : block(block_), obj(object) {
        if constexpr (std::is_convertible_v<T*, EnableSharedFromThisBase*>) {
            InitWeakThis(object);
        }
    }

    template <typename U>
    void InitWeakThis(EnableSharedFromThis<U>* e) {
        e->weak_this = *this;
    }

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

    template <typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args&&...);
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
}

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    void* buffer = nullptr;
    try {
        buffer = ::operator new(sizeof(ControlBlockObjectImp<T>));
        ControlBlockObjectImp<T>* block =
            new (buffer) ControlBlockObjectImp<T>(std::forward<Args>(args)...);

        return SharedPtr<T>(static_cast<ControlBlock*>(block), block->GetObject());
    } catch (...) {
        ::operator delete(buffer);
        throw;
    }
}

class EnableSharedFromThisBase {};

template <typename T>
class EnableSharedFromThis : public EnableSharedFromThisBase {
public:
    EnableSharedFromThis() = default;
    EnableSharedFromThis(const EnableSharedFromThis&) = delete;
    EnableSharedFromThis& operator=(const EnableSharedFromThis&) = delete;
    EnableSharedFromThis(EnableSharedFromThis&&) = delete;
    EnableSharedFromThis& operator=(EnableSharedFromThis&&) = delete;
    virtual ~EnableSharedFromThis() {
    }

    SharedPtr<T> SharedFromThis() {
        auto shared = weak_this.Lock();
        if (!shared) {
            throw BadWeakPtr();
        }
        return shared;
    }
    SharedPtr<const T> SharedFromThis() const {
        auto shared = weak_this.Lock();
        if (!shared) {
            throw BadWeakPtr();
        }
        return shared;
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return weak_this;
    }
    WeakPtr<const T> WeakFromThis() const noexcept {
        return WeakPtr<const T>(weak_this);
    }

private:
    WeakPtr<T> weak_this;

    template <typename U>
    friend class SharedPtr;
};
