#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "rtweekend.h"

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t {0};
    double u;
    double v;
    bool front_face {true};

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;
};

class translate : public hittable {
public:
    translate(shared_ptr<hittable> p, const vec3& displacement)
        : ptr(std::move(p)), offset(displacement) {}

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
    shared_ptr<hittable> ptr;
    vec3 offset;
};

bool translate::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

bool translate::bounding_box(double time0, double time1, aabb &output_box) const {
    if (!ptr->bounding_box(time0, time1, output_box))
        return false;

    output_box = aabb {
        output_box.min() + offset,
        output_box.max() + offset
    };

    return true;
}

class rotate_y : public hittable {
public:
    rotate_y(shared_ptr<hittable> p, double angle);

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        output_box = bbox;
        return hasbox;
    }

public:
    shared_ptr<hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    aabb bbox;
};

rotate_y::rotate_y(shared_ptr<hittable> p, double angle) : ptr(std::move(p)) {
    double radians = degrees_to_radians(angle);
    this->sin_theta = sin(radians);
    this->cos_theta = cos(radians);
    this->hasbox = ptr->bounding_box(0, 1, this->bbox);

    point3 min { infinity, infinity, infinity };
    point3 max { -infinity, -infinity, -infinity };

    for (int i=0; i<2; ++i)
        for (int j=0; j<2; ++j)
            for (int k=0; k<2; ++k) {
                double x = i*bbox.max().x() + (1-i)*bbox.min().x();
                double y = j*bbox.max().y() + (1-j)*bbox.min().y();
                double z = k*bbox.max().z() + (1-k)*bbox.min().z();

                double newx = this->cos_theta*x + this->sin_theta*z;
                double newz = -this->sin_theta*x + this->cos_theta*z;

                vec3 tester { newx, y, newz };

                for (int c = 0; c < 3; ++c) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }

    bbox = aabb(min, max);
}

bool rotate_y::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    point3 origin {
        this->cos_theta * r.origin()[0] - this->sin_theta * r.origin()[2],
        r.origin()[1],
        this->sin_theta * r.origin()[0] + this->cos_theta * r.origin()[2]
    };
    vec3 direction = {
            this->cos_theta * r.direction()[0] - this->sin_theta * r.direction()[2],
            r.direction()[1],
            this->sin_theta * r.direction()[0] + this->cos_theta * r.direction()[2]
    };

    ray rotated_r { origin, direction, r.time() };

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    point3 p {
        this->cos_theta * rec.p[0] + this->sin_theta * rec.p[2],
        rec.p[1],
        -this->sin_theta * rec.p[0] + this->cos_theta * rec.p[2]
    };
    vec3 normal {
            this->cos_theta * rec.normal[0] + this->sin_theta * rec.normal[2],
            rec.normal[1],
            -this->sin_theta * rec.normal[0] + this->cos_theta * rec.normal[2]
    };

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}

class flip_face : public hittable {
public:
    explicit flip_face(shared_ptr<hittable> p) : ptr(std::move(p)) {}

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        if (!ptr->hit(r, t_min, t_max, rec))
            return false;

        rec.front_face = !rec.front_face;
        return true;
    }

    bool bounding_box(double time0, double time1, aabb& output_box) const override {
        return ptr->bounding_box(time0, time1, output_box);
    }

public:
    shared_ptr<hittable> ptr;
};

#endif
