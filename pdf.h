#ifndef PDF_H
#define PDF_H

#include "rtweekend.h"
#include "onb.h"

inline vec3 random_to_sphere(double radius, double distance_squared) {
    double r1 = random_double();
    double r2 = random_double();

    double cos_thetamax { sqrt(1 - radius*radius / distance_squared) };
    double cos_theta = 1 + r2*(cos_thetamax - 1);

    double z = cos_theta;
    double x = cos(2*pi*r1) * sqrt(1 - z*z);
    double y = sin(2*pi*r1) * sqrt(1 - z*z);

    return { x, y, z };
}

class pdf {
public:
    virtual ~pdf() = default;

    [[nodiscard]] virtual double value(const vec3& direction) const = 0;
    [[nodiscard]] virtual vec3 generate() const = 0;
};

inline vec3 random_cosine_direction() {
    double r1 = random_double();
    double r2 = random_double();
    double z = sqrt(1-r2);

    double phi = 2*pi*r1;
    double x = cos(phi)*sqrt(r2);
    double y = sin(phi)*sqrt(r2);

    return { x, y, z };
}

class cosine_pdf : public pdf {
public:
    explicit cosine_pdf(const vec3& w) { uvw.build_from_w(w); }

    [[nodiscard]] double value(const vec3& direction) const override {
        double cosine = dot(unit_vector(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine / pi;
    }

    [[nodiscard]] vec3 generate() const override {
        return uvw.local(random_cosine_direction());
    }

public:
    onb uvw;
};

class hittable_pdf : public pdf {
public:
    hittable_pdf(shared_ptr<hittable> p, const point3& origin) : ptr(std::move(p)), o(origin) {}

    [[nodiscard]] double value(const vec3& direction) const override {
        return ptr->pdf_value(o, direction);
    }

    [[nodiscard]] vec3 generate() const override {
        return ptr->random(o);
    }

public:
    point3 o;
    shared_ptr<hittable> ptr;
};

class mixture_pdf : public pdf {
public:
    mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) {
        p[0] = std::move(p0);
        p[1] = std::move(p1);
    }

    [[nodiscard]] double value(const vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    [[nodiscard]] vec3 generate() const override {
        if (random_double() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }

public:
    shared_ptr<pdf> p[2];
};

#endif //PDF_H
