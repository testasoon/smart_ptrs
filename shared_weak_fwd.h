#pragma once

#include <exception>

class BadWeakPtr;

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

class ControlBlock;

template <typename T>
class ControlBlockPointerImp;

template <typename T>
class ControlBlockObjectImp;

class EnableSharedFromThisBase;

template <typename T>
class EnableSharedFromThis;
