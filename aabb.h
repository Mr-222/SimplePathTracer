#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb {
public:
    aabb() = delete;
    aabb(const point3& a, const point3& b) : minimum(a), maximum(b)  {}

    [[nodiscard]] point3 min() const { return minimum; }
    [[nodiscard]] point3 max() const { return maximum; }

    [[nodiscard]] bool hit(const ray& r, double t_min, double t_max) const {
        for (int a=0; a<3; a++) {
            double invD = 1.0f / r.direction()[a];
            double t0 = (min()[a] - r.origin()[a]) * invD;
            double t1 = (max()[a] - r.origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

public:
    point3 minimum;
    point3 maximum;
};

#endif //AABB_H
