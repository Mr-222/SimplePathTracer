#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"

class texture {
public:
    [[nodiscard]] virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
    solid_color() = default;
    explicit solid_color(const color& c) : color_value(c) {}

    solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

    [[nodiscard]] color value(double u, double v, const vec3& p) const override {
        return color_value;
    }

private:
    color color_value;
};

class checker_texture : public texture {
public:
    checker_texture() = default;

    checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : even(_even), odd(_odd) {}

    checker_texture(const color& c1, const color& c2)
        : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

    [[nodiscard]] color value(double u, double v, const point3& p) const override {
        double sines = sin(10*p.x()) * sin(10*p.y()) * sin(10*p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
};

#endif //TEXTURE_H
