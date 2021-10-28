#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere() = delete;
    sphere(point3 cen, double r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m) {};
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray &r,double t_min, double t_max, hit_record &rec) const {
    vec3 oc = r.origin() - center;
    double a = r.direction().length_squared();
    double half_b = dot(oc, r.direction());
    double c = oc.length_squared() - radius*radius;

    double discriminant = half_b*half_b - a*c;
    if (discriminant < 0)
        return false;
    double sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    double root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(root);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool sphere::bounding_box(double time0, double time1, aabb &output_box) const {
    output_box = aabb {
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius)
    };
    return true;
}

#endif
