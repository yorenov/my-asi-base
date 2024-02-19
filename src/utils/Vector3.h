#pragma once
#include <cstdint>
#include <stdexcept>

template<typename ValueType>
class Vector3 {
public:
    ValueType x, y, z;

    Vector3() : Vector3(0, 0, 0) {}

    template<typename NumberType>
    Vector3(NumberType x, NumberType y, NumberType z) {
        this->x = static_cast<ValueType>(x);
        this->y = static_cast<ValueType>(y);
        this->z = static_cast<ValueType>(z);
    }

    template<typename NumberType>
    Vector3(Vector3<NumberType> otherType) : Vector3(otherType.x, otherType.y, otherType.z) {}

    template<class CustomVector>
    Vector3(const CustomVector& customVector) : Vector3(customVector.x, customVector.y, customVector.z) {}

    constexpr ValueType& operator[](uint8_t index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::range_error("Index out of bounds!");
        }
    }
    constexpr ValueType operator[](uint8_t index) const {
        return operator[](index);
    }

    [[nodiscard]] bool isZero() const {
        return x == 0 && y == 0 && z == 0;
    }

    [[nodiscard]] ValueType magnitude() {
        return sqrt(x * x + y * y + z * z);
    }

    void normalize() {
        long double sum = x * x + y * y + z * z;

        if (sum > 0)
        {
            long double d = 1.0 / sqrt(sum);
            x = d * x;
            y = d * y;
            z = d * z;
        }
        else
            x = 1.0;
    }

    template<class OtherVector, std::enable_if_t<std::is_class_v<OtherVector>, bool> = true>
    Vector3<ValueType> cross(const OtherVector& b) const {
        return {
                y * b.z - z * b.y,
                z * b.x - x * b.z,
                x * b.y - y * b.x
        };
    }

    template<typename NumberType>
    Vector3<NumberType> convert() const {
        return {x, y, z};
    }

    template<class CustomVector, std::enable_if_t<std::is_class_v<CustomVector>, bool> = true>
    [[nodiscard]] float distance(const CustomVector& b) {
        return sqrtf((x - b.x) * (x - b.x) + (y - b.y) * (y - b.y) + (z - b.z) * (z - b.z));
    }

    // Basic operators
    template<typename NumberType, std::enable_if_t<std::is_arithmetic<NumberType>::value, bool> = true>
    Vector3& operator=(NumberType b) {
        x = b;
        y = b;
        z = b;

        return *this;
    }

    template<typename NumberType>
    Vector3& operator+=(NumberType b) {
        auto _b = static_cast<ValueType>(b);
        x += _b;
        y += _b;
        z += _b;

        return *this;
    }
    template<typename NumberType>
    Vector3& operator-=(NumberType b) {
        auto _b = static_cast<ValueType>(b);
        x -= _b;
        y -= _b;
        z -= _b;

        return *this;
    }
    template<typename NumberType>
    Vector3& operator*=(NumberType b) {
        auto _b = static_cast<ValueType>(b);
        x *= _b;
        y *= _b;
        z *= _b;

        return *this;
    }
    template<typename NumberType>
    Vector3& operator/=(NumberType b) {
        auto _b = static_cast<ValueType>(b);
        x /= _b;
        y /= _b;
        z /= _b;

        return *this;
    }

    template<typename NumberType>
    Vector3 operator+(NumberType b) const {
        auto _b = static_cast<ValueType>(b);

        return {x + b, y + b, z + b};
    }
    template<typename NumberType>
    Vector3 operator-(NumberType b) const {
        auto _b = static_cast<ValueType>(b);

        return {x - b, y - b, z - b};
    }
    template<typename NumberType>
    Vector3 operator*(NumberType b) const {
        auto _b = static_cast<ValueType>(b);

        return {x * b, y * b, z * b};
    }
    template<typename NumberType>
    Vector3 operator/(NumberType b) const {
        auto _b = static_cast<ValueType>(b);

        return {x / b, y / b, z / b};
    }

    // Class operators
    template<class CustomVector, std::enable_if_t<std::is_class<CustomVector>::value, bool> = true>
    Vector3& operator=(const CustomVector& b) {
        x = b.x;
        y = b.y;
        z = b.z;

        return *this;
    }

    Vector3& operator=(const Vector3& b) {
        x = b.x;
        y = b.y;
        z = b.z;

        return *this;
    }

    Vector3& operator+=(const Vector3& b) {
        x += b.x;
        y += b.y;
        z += b.z;

        return *this;
    }
    Vector3& operator-=(const Vector3& b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;

        return *this;
    }
    Vector3& operator*=(const Vector3& b) {
        x *= b.x;
        y *= b.y;
        z *= b.z;

        return *this;
    }
    Vector3& operator/=(const Vector3& b) {
        x /= b.x;
        y /= b.y;
        z /= b.z;

        return *this;
    }

    Vector3 operator+(const Vector3& b) const {
        return {x + b.x, y + b.y, z + b.z};
    }
    Vector3 operator-(const Vector3& b) const {
        return {x - b.x, y - b.y, z - b.z};
    }
    Vector3 operator*(const Vector3& b) const {
        return {x * b.x, y * b.y, z * b.z};
    }
    Vector3 operator/(const Vector3& b) const {
        return {x / b.x, y / b.y, z / b.z};
    }
};

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
using Vector3i = Vector3<int>;