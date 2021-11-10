#ifndef SPHERE_H
#define SPHERE_H

#include <utility>

#include "hittable.h"
#include "vec3.h"
#include "onb.h"
#include "pdf.h"

class sphere : public hittable {
public:
    sphere() = delete;
    sphere(point3 cen, double r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(std::move(m)) {};
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    bool bounding_box(double time0, double time1, aabb& output_box) const override;
    [[nodiscard]] double pdf_value(const point3& o, const vec3& v) const override;

    [[nodiscard]] vec3 random(const point3& o) const override;

public:
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;

private:
    static void get_sphere_uv(const point3& p, double &u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        double theta = acos(-p.y());
        double phi = atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
    }
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
    vec3  outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
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

double sphere::pdf_value(const point3& o, const vec3& v) const {
    hit_record rec;
    if (!this->hit(ray(o, v), 0.001, infinity, rec))
        return 0.0;

    double cos_thetamax { sqrt(1 - this->radius*this->radius/(this->center-o).length_squared()) };
    return 1.0 / 2 * pi * (1-cos_thetamax);
}

vec3 sphere::random(const point3& o) const {
    vec3 direction = this->center - o;
    onb uvw;
    uvw.build_from_w(direction);
    double distance_squared { direction.length_squared() };
    return uvw.local(random_to_sphere(this->radius, distance_squared));
}

#endif
