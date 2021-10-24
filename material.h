#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"

class material {
public:
    virtual bool scatter (
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
};

class lambertian : public material {
public:
    explicit lambertian(const color& a) : albedo(a) {}
    bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
            ) const override {
        vec3 scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
    color albedo;
};

#endif
