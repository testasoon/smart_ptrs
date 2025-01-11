#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t

template <typename T>
struct DefaultDeleter {
    DefaultDeleter() noexcept = default;

    template <typename Tp, typename = typename std::enable_if_t<std::is_convertible_v<Tp*, T*>>>
    DefaultDeleter(const DefaultDeleter<Tp>&) noexcept {
    }

    void operator()(T* ptr_) const {
        static_assert(!std::is_void_v<T>);
        static_assert(sizeof(T) > 0);
        delete ptr_;
    }
};

template <typename T>
struct DefaultDeleter<T[]> {
    DefaultDeleter() noexcept = default;

    void operator()(T* ptr_) const {
        static_assert(sizeof(T) > 0);
        delete[] ptr_;
    }
};

template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr) noexcept : UniquePtr(ptr, Deleter()) {
    }
    UniquePtr(T* ptr, Deleter&& deleter) noexcept : data_(ptr, std::forward<Deleter>(deleter)) {
    }

    UniquePtr(T* ptr, const Deleter& deleter) noexcept : data_(ptr, deleter) {
    }

    UniquePtr(UniquePtr&& other) noexcept
        : data_(other.Release(), std::forward<Deleter>(other.GetDeleter())) {
    }

    template <typename Tp, typename DeleterType,
              typename = typename std::enable_if_t<
                  std::__and_v<std::is_convertible<Tp*, T*>, std::__not_<std::is_array<Tp>>>>>
    UniquePtr(UniquePtr<Tp, DeleterType>&& other) noexcept
        : data_(other.Release(), std::forward<Deleter>(other.GetDeleter())) {
    }

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        GetDeleter() = std::forward<Deleter>(other.GetDeleter());
        return *this;
    }

    template <typename Tp, typename DeleterType,
              typename = typename std::enable_if_t<
                  std::__and_v<std::is_convertible<Tp*, T*>, std::__not_<std::is_array<Tp>>>>>
    UniquePtr& operator=(UniquePtr<Tp, DeleterType>&& other) noexcept {
        Reset(other.Release());
        GetDeleter() = std::forward<Deleter>(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        auto& ptr_ = data_.GetFirst();
        if (ptr_ != nullptr) {
            GetDeleter()(ptr_);
            ptr_ = nullptr;
        }
    }

    T* Release() {
        T* ptr = Get();
        data_.GetFirst() = nullptr;
        return ptr;
    }

    void Reset(T* ptr = nullptr) {
        std::swap(ptr, data_.GetFirst());
        if (ptr != nullptr) {
            GetDeleter()(ptr);
        }
    }

    void Swap(UniquePtr& other) {
        std::swap(data_, other.data_);
    }

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return Get() != nullptr;
    }

    typename std::add_lvalue_reference_t<T> operator*() const {
        static_assert(!std::is_same_v<T, void>);
        return *(Get());
    }

    T* operator->() const {
        return Get();
    }

private:
    CompressedPair<T*, Deleter> data_;
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    explicit UniquePtr(T* ptr = nullptr) noexcept : UniquePtr(ptr, Deleter()) {
    }
    UniquePtr(T* ptr, Deleter&& deleter) noexcept : data_(ptr, std::forward<Deleter>(deleter)) {
    }

    UniquePtr(T* ptr, const Deleter& deleter) noexcept : data_(ptr, deleter) {
    }

    UniquePtr(UniquePtr&& other) noexcept
        : data_(other.Release(), std::forward<Deleter>(other.GetDeleter())) {
    }

    template <typename Tp, typename DeleterType,
              typename = typename std::enable_if_t<
                  std::__and_v<std::is_convertible<Tp*, T*>, std::__not_<std::is_array<Tp>>>>>
    UniquePtr(UniquePtr<Tp, DeleterType>&& other) noexcept
        : data_(other.Release(), std::forward<Deleter>(other.GetDeleter())) {
    }

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        GetDeleter() = std::forward<Deleter>(other.GetDeleter());
        return *this;
    }

    template <typename Tp, typename DeleterType,
              typename = typename std::enable_if_t<
                  std::__and_v<std::is_convertible<Tp*, T*>, std::__not_<std::is_array<Tp>>>>>
    UniquePtr& operator=(UniquePtr<Tp, DeleterType>&& other) noexcept {
        Reset(other.Release());
        GetDeleter() = std::forward<Deleter>(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        auto& ptr_ = data_.GetFirst();
        if (ptr_ != nullptr) {
            GetDeleter()(ptr_);
            ptr_ = nullptr;
        }
    }

    T* Release() {
        T* ptr = Get();
        data_.GetFirst() = nullptr;
        return ptr;
    }

    void Reset(T* ptr = nullptr) {
        std::swap(ptr, data_.GetFirst());
        if (ptr != nullptr) {
            GetDeleter()(ptr);
        }
    }

    void Swap(UniquePtr& other) {
        std::swap(data_, other.data_);
    }

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return Get() != nullptr;
    }

    typename std::add_lvalue_reference_t<T> operator*() const {
        static_assert(!std::is_same_v<T, void>);
        return *(Get());
    }

    T* operator->() const {
        return Get();
    }

    const T& operator[](const size_t& ind) const {
        return Get()[ind];
    }

    T& operator[](const size_t& ind) {
        return Get()[ind];
    }

private:
    CompressedPair<T*, Deleter> data_;
};