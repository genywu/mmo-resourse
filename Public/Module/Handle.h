#ifndef HANDLE_H
#define HANDLE_H

template<typename T>
class Handle
{
public:
    // unbound handle
    Handle(T* p=0) : ptr(p), use(new size_t(1)) {}

    // overloaded operators to support pointer behavior
    T& operator*();
    T* operator->();
    const T& operator*() const;
    const T* operator->() const;

    // copy control: normal pointer behavior, but last Handle deletes the object
    Handle(const Handle& h) : ptr(h.ptr), use(h.use)
    {++*use;}
    Handle& operator=(const Handle&);
    ~Handle() {rem_ref();}

    // is the handle bound or unbound?
    bool Bound() const {return ptr!=0;}
    bool Unbound() const {return ptr==0;}

private:
    T* ptr;
    size_t* use;

    // decrement use count and delete pointers if needed
    // used for destructor and assignment operator
    void rem_ref()
    {
        if (--*use == 0)
        {
            delete ptr;
            delete use;
        }
    }
};

template<typename T>
inline Handle<T>& Handle<T>::operator=(const Handle& rhs)
{
    ++*rhs.use;     // protect against self-assignment
    rem_ref();      // decrement use count and delete pointers if needed
    ptr = rhs.ptr;
    use = rhs.use;
    return *this;
}

template<typename T> inline T& Handle<T>::operator*()
{
    if (ptr)
        return *ptr;
    throw std::runtime_error("dereference of unbound Handle");
}

template<typename T> inline T* Handle<T>::operator->()
{
    if (ptr)
        return ptr;
    throw std::runtime_error("access through unbound Handle");
}

template<typename T> inline const T& Handle<T>::operator*() const
{
    if (ptr)
        return *ptr;
    throw std::runtime_error("dereference of unbound Handle");
}

template<typename T> inline const T* Handle<T>::operator->() const
{
    if (ptr)
        return ptr;
    throw std::runtime_error("access through unbound Handle");
}

#endif//#ifndef HANDLE_H