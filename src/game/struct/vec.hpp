#pragma once
#include <string>

#pragma pack(push, 1)
// We don't need full-fat CL_Vec classes, use more lean structs.
template<typename Type>
class Vec2
{
  public:
    Vec2()
    {
        x = 0;
        y = 0;
    }
    Vec2(Type n)
    {
        x = n;
        y = n;
    }
    Vec2(Type x, Type y)
    {
        this->x = x;
        this->y = y;
    }
    float x, y;
    bool operator==(const Vec2& rhs) const { return x == rhs.x && y == rhs.y; }
    void operator-=(const Vec2& vector)
    {
        x -= vector.x;
        y -= vector.y;
    }
    void operator-=(Type value)
    {
        x -= value;
        y -= value;
    }
    Vec2 operator-(const Vec2& vector) const { return Vec2(x - vector.x, y - vector.y); }
    Vec2 operator-(Type value) const { return Vec2(x - value, y - value); }
    Vec2 operator-() const { return Vec2(-x, -y); }
    std::string Print()
    {
        // Borrowed from Proton.
        char st[128];
        sprintf(st, "%.2f, %.2f", this->x, this->y);
        return std::string(st);
    }
};

class Vec3f
{
  public:
    Vec3f()
    {
        x = 0;
        y = 0;
        z = 0;
    }
    Vec3f(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    float x, y, z;
    bool operator==(const Vec3f& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    std::string Print()
    {
        // Borrowed from Proton.
        char st[128];
        sprintf(st, "%.3f, %.3f, %.3f", this->x, this->y, this->z);
        return std::string(st);
    }
};
class Rectf
{
  public:
    Rectf()
    {
        left = 0;
        top = 0;
        right = 0;
        bottom = 0;
    }
    Rectf(float left, float top, float right, float bottom)
    {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }
    void ceil()
    {
        this->left = ceilf(this->left);
        this->top = ceilf(this->top);
        this->right = ceilf(this->right);
        this->bottom = ceilf(this->bottom);
    }

    float left, top, right, bottom;
    std::string Print()
    {
        // Borrowed from Proton.
        char st[128];
        sprintf(st, "%.3f, %.3f, %.3f, %.3f", this->left, this->top, this->right, this->bottom);
        return std::string(st);
    }
};

// Map these to CL_ naming convention.
typedef Vec2<float> CL_Vec2f;
typedef Vec2<int> CL_Vec2i;
typedef Vec3f CL_Vec3f;
typedef Rectf CL_Rectf;
#pragma pack(pop)