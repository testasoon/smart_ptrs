#pragma once

#include <memory>
#include <type_traits>

template <typename F, typename S, bool IsSame, bool FirstEmpty,
          bool SecondEmpty>
struct Switch;

template <typename F, typename S> struct Switch<F, S, false, false, false> {
  static const int value = 0;
};

template <typename F, typename S> struct Switch<F, S, true, true, false> {
  static const int value = 0;
};

template <typename F, typename S> struct Switch<F, S, true, false, true> {
  static const int value = 0;
};

template <typename F, typename S> struct Switch<F, S, false, true, false> {
  static const int value = 1;
};

template <typename F, typename S> struct Switch<F, S, false, false, true> {
  static const int value = 2;
};

template <typename F, typename S> struct Switch<F, S, false, true, true> {
  static const int value = 3;
};

template <typename F, typename S> struct Switch<F, S, true, true, true> {
  static const int value = 4;
};

template <typename F, typename S> struct Switch<F, S, true, false, false> {
  static const int value = 5;
};

template <typename F, typename S, int Version> class CompressedPairImp;

// F != {} and S != {} and F != S
template <typename F, typename S> class CompressedPairImp<F, S, 0> {
public:
  CompressedPairImp() {}

  CompressedPairImp(const F &first, const S &second)
      : first_(first), second_(second) {}

  CompressedPairImp(F &&first, S &&second)
      : first_(std::forward<F>(first)), second_(std::forward<S>(second)) {}

  CompressedPairImp(const F &first, S &&second)
      : first_(first), second_(std::forward<S>(second)) {}

  CompressedPairImp(F &&first, const S &second)
      : first_(std::forward<F>(first)), second_(second) {}

  F &GetFirst() { return first_; }

  const F &GetFirst() const { return first_; }

  S &GetSecond() { return second_; }

  const S &GetSecond() const { return second_; }

private:
  F first_;
  S second_;
};

// only F is {}
template <typename F, typename S> class CompressedPairImp<F, S, 1> : F {
public:
  CompressedPairImp() {}

  CompressedPairImp(const F &first, const S &second)
      : F(first), second_(second) {}

  CompressedPairImp(F &&first, S &&second)
      : F(std::forward<F>(first)), second_(std::forward<S>(second)) {}

  CompressedPairImp(const F &first, S &&second)
      : F(first), second_(std::forward<S>(second)) {}

  CompressedPairImp(F &&first, const S &second)
      : F(std::forward<F>(first)), second_(second) {}

  F &GetFirst() { return *this; }

  const F &GetFirst() const { return *this; }

  S &GetSecond() { return second_; }

  const S &GetSecond() const { return second_; }

private:
  S second_;
};

// only S is {}
template <typename F, typename S> class CompressedPairImp<F, S, 2> : S {
public:
  CompressedPairImp() {}

  CompressedPairImp(const F &first, const S &second)
      : S(second), first_(first) {}

  CompressedPairImp(F &&first, S &&second)
      : S(std::forward<S>(second)), first_(std::forward<F>(first)) {}

  CompressedPairImp(const F &first, S &&second)
      : S(std::forward<S>(second)), first_(first) {}

  CompressedPairImp(F &&first, const S &second)
      : S(second), first_(std::forward<F>(first)) {}

  F &GetFirst() { return first_; }

  const F &GetFirst() const { return first_; }

  S &GetSecond() { return *this; }

  const S &GetSecond() const { return *this; }

private:
  F first_;
};

// F != S F is {} and S is {}
template <typename F, typename S> class CompressedPairImp<F, S, 3> : F, S {
public:
  CompressedPairImp() {}

  CompressedPairImp(const F &first, const S &second) : F(first), S(second) {}

  CompressedPairImp(F &&first, S &&second)
      : F(std::forward<F>(first)), S(std::forward<S>(second)) {}

  CompressedPairImp(const F &first, S &&second)
      : F(first), S(std::forward<S>(second)) {}

  CompressedPairImp(F &&first, const S &second)
      : F(std::forward<F>(first)), S(second) {}

  F &GetFirst() { return *this; }

  const F &GetFirst() const { return *this; }

  S &GetSecond() { return *this; }

  const S &GetSecond() const { return *this; }
};

// F == S and F is {} and S is {}
template <typename F, typename S> class CompressedPairImp<F, S, 4> : F {
public:
  CompressedPairImp() {}

  CompressedPairImp(const F &first, const S &second)
      : F(first), second_(second) {}

  CompressedPairImp(F &&first, S &&second)
      : F(std::forward<F>(first)), second_(std::forward<F>(second)) {}

  CompressedPairImp(const F &first, S &&second)
      : F(first), second_(std::forward<F>(second)) {}

  CompressedPairImp(F &&first, const S &second)
      : F(std::forward<F>(first)), second_(second) {}

  F &GetFirst() { return *this; }

  const F &GetFirst() const { return *this; }

  S &GetSecond() { return *(this + 1); }

  const S &GetSecond() const { return *(this + 1); }

private:
  S second_;
};

// F == S and F != {}
template <typename F, typename S> class CompressedPairImp<F, S, 5> {
public:
  CompressedPairImp() {}

  CompressedPairImp(const F &first, const S &second)
      : first_(first), second_(second) {}

  CompressedPairImp(F &&first, S &&second)
      : first_(std::forward<F>(first)), second_(std::forward<S>(second)) {}

  CompressedPairImp(const F &first, S &&second)
      : first_(first), second_(std::forward<S>(second)) {}

  CompressedPairImp(F &&first, const S &second)
      : first_(std::forward<F>(first)), second_(second) {}

  F &GetFirst() { return first_; }

  const F &GetFirst() const { return first_; }

  S &GetSecond() { return second_; }

  const S &GetSecond() const { return second_; }

private:
  F first_;
  S second_;
};

template <typename T>
inline constexpr bool is_empty_and_not_final =
    std::is_empty_v<T> && !std::is_final_v<T>;

template <typename F, typename S>
inline constexpr bool is_base =
    std::is_base_of_v<F, S> || std::is_base_of_v<S, F>;

template <typename F, typename S>
class CompressedPair
    : public CompressedPairImp<
          F, S,
          Switch<F, S, is_base<F, S>, is_empty_and_not_final<F>,
                 is_empty_and_not_final<S>>::value> {
public:
  typedef CompressedPairImp<
      F, S,
      Switch<F, S, is_base<F, S>, is_empty_and_not_final<F>,
             is_empty_and_not_final<S>>::value>
      base_;

  CompressedPair() : base_() {}

  CompressedPair(const F &first, const S &second) : base_(first, second) {}

  CompressedPair(F &&first, S &&second)
      : base_(std::forward<F>(first), std::forward<S>(second)) {};

  CompressedPair(const F &first, S &&second)
      : base_(first, std::forward<S>(second)) {}

  CompressedPair(F &&first, const S &second)
      : base_(std::forward<F>(first), second) {}

  F &GetFirst() { return base_::GetFirst(); }

  const F &GetFirst() const { return base_::GetFirst(); }

  S &GetSecond() { return base_::GetSecond(); }

  const S &GetSecond() const { return base_::GetSecond(); }
};