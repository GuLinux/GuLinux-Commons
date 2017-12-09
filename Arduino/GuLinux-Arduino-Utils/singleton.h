// This is actually a pseudo singleton class.
// The class has to be initialized manually a first time
// Every further reference to `instance()` will refer to the first initialized variable.
#pragma once

template<typename T> class Singleton {
public:
  Singleton(T *instance) { this->_instance = instance; }
  static T *instance() { return _instance; }
private:
  static T *_instance;
};

template<typename T> T *Singleton<T>::_instance = nullptr;
