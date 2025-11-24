#pragma once

namespace ssge
{
    template <class T>
    // PassKeys are used to restrict method/ctor calls to specific classes.
    // It's friend class for methods, basically
    class PassKey
    {
        friend T;                 // Only T can create PassKey<T>
        PassKey() = default;      // Not public
    public:
        PassKey(const PassKey&) = default;
    };
}