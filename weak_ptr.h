#pragma once

#include "shared_weak_fwd.h"
#include "control_block.h"

template <typename T>
class WeakPtr {
public:
    WeakPtr() noexcept : block(nullptr), obj(nullptr) {
    }

    WeakPtr(const WeakPtr& other) noexcept : block(other.block), obj(other.obj) {
        if (block)
            block->IncWeakRef();
    }

    template <typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
    WeakPtr(const WeakPtr<U>& other) noexcept : block(other.block), obj(other.obj) {
        if (block)
            block->IncWeakRef();
    }

    WeakPtr(WeakPtr&& other) noexcept : block(other.block), obj(other.obj) {
        other.block = nullptr;
        other.obj = nullptr;
    }

    template <typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
    WeakPtr(WeakPtr<U>&& other) noexcept : block(other.block), obj(other.obj) {
        other.block = nullptr;
        other.obj = nullptr;
    }

    WeakPtr(const SharedPtr<T>& other) noexcept : block(other.block), obj(other.obj) {
        if (block)
            block->IncWeakRef();
    }

    template <typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
    WeakPtr(const SharedPtr<U>& other) : block(other.block), obj(other.obj) {
        if (block)
            block->IncWeakRef();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this == &other) {
            return *this;
        }
        if (block)
            block->DecWeakRef();

        block = other.block;
        obj = other.obj;
        if (block)
            block->IncWeakRef();
        return *this;
    }

    template <typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
    WeakPtr& operator=(const SharedPtr<U>& other) {
        if (block)
            block->DecWeakRef();

        block = other.block;
        obj = other.obj;
        if (block)
            block->IncWeakRef();
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (this == &other) {
            return *this;
        }

        if (block)
            block->DecWeakRef();

        block = other.block;
        obj = other.obj;
        other.obj = nullptr;
        other.block = nullptr;
        return *this;
    }

    ~WeakPtr() {
        if (block)
            block->DecWeakRef();
    }

    void Reset() {
        if (block)
            block->DecWeakRef();

        block = nullptr;
        obj = nullptr;
    }
    void Swap(WeakPtr& other) {
        std::swap(block, other.block);
        std::swap(obj, other.obj);
    }

    size_t UseCount() const {
        if (!block)
            return 0;
        return block->UseCount();
    }
    bool Expired() const {
        return UseCount() == 0;
    }
    SharedPtr<T> Lock() const {
        return SharedPtr<T>(*this, std::nothrow);
    }

private:
    ControlBlock* block;
    T* obj;

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;
};
