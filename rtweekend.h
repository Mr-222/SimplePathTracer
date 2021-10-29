#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double(double min=0.0, double max=1.0) {
    // Returns a random real in [min,max).
    static std::once_flag flag;
    static std::mt19937 generator;
    std::call_once(flag ,[&]() {
        generator.seed(std::random_device{}());
    });
    static std::uniform_real_distribution<double> dis(min, max);
    return dis(generator);
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min, max].
    return static_cast<int>(random_double(min, max+1));
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers
#include "ray.h"
#include "vec3.h"

#endif
