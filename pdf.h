#ifndef PDF_H
#define PDF_H

#include "rtweekend.h"
#include "onb.h"

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

#endif //PDF_H
