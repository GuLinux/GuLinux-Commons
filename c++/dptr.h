/*
 * Copyright (C) 2016 Marco Gulino <marco@gulinux.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef CPP_DPTR_H
#define CPP_DPTR_H

#if __cplusplus >= 201103L
// Use std::unique_ptr instead of custom ptr class
// Use new uniform initializer to avoid unnecessary constructor definition
#include <memory>
template<typename T> using UniqueDPTR = std::unique_ptr<T>;
#define dptr(...) d(new Private{__VA_ARGS__})

#else // __cplusplus
// Simple std::unique_ptr replacement class
// Doesn't aim to be a fully featured unique_ptr, but just a very basic ptr wrapper with automatic deletion for dptr.
template<typename T> class UniqueDPTR {
public:
    UniqueDPTR(T *t) : ptr(t) {}
    ~UniqueDPTR() { delete ptr; }
    T *get() const { return ptr; }
    T &operator*() const { return *get(); }
    T *operator->() const { return get(); }
private:
    T *ptr;
    UniqueDPTR(UniqueDPTR &); // Disable copy constructor
    UniqueDPTR &operator=(const UniqueDPTR &); // Disable assignment
};

#define dptr(...) d(new Private(__VA_ARGS__))

#endif // __cplusplus

#define DPTR_IMPL(_class) struct _class::Private
#define DPTR struct Private; friend struct Private; const UniqueDPTR<Private> d;

#endif // CPP_DPTR_H

