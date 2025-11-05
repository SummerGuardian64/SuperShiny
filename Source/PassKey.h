#pragma once

namespace ssge {
    template <class T>
    class PassKey {
        friend T;                 // Only T can create PassKey<T>
        PassKey() = default;      // Not public
    public:
        PassKey(const PassKey&) = default;
    };
}