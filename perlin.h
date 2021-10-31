#ifndef PERLIN_H
#define PERLIN_H

#include <array>
#include "rtweekend.h"

class perlin {
public:
    perlin() {
        ranfloat = new double[point_count];
        for (int i=0; i<point_count; ++i) {
            ranfloat[i] = random_double();
        }

        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin() {
        delete[] ranfloat;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    [[nodiscard]] double noise(const point3& p) const {
        double u = p.x() - floor(p.x());
        double v = p.y() - floor(p.y());
        double w = p.z() - floor(p.z());

        int i = static_cast<int>(floor(p.x()));
        int j = static_cast<int>(floor(p.y()));
        int k = static_cast<int>(floor(p.z()));
        double c[2][2][2];

        for (int di=0; di<2; di++)
            for (int dj=0; dj<2; dj++)
                for (int dk=0; dk<2; dk++)
                    c[di][dj][dk] = ranfloat[
                            perm_x[(i+di) & 255] ^
                            perm_y[(j+dj) & 255] ^
                            perm_z[(k+dk) & 255]
                    ];

        return trilinear_interp(c, u, v, w);
    }

private:
    static const int point_count = 256;
    double* ranfloat;
    int* perm_x;
    int* perm_y;
    int* perm_z;

    static int* perlin_generate_perm() {
        auto p = new std::array<int, point_count> {};

        int j = 0;
        std::for_each(p->begin(), p->end(), [&j](int& i) {
            i = j;
            j++;
        });

        permute(p->data(), point_count);

        return p->data();
    }

    static void permute(int* p, int n) {
        for (int i=n-1; i>0; i--) {
            int target = random_int(0, i);
            std::swap(p[i], p[target]);
        }
    }

    static double trilinear_interp(double c[2][2][2], double u, double v, double w) {
        double accum = 0.0;
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
                for (int k = 0; k < 2; ++k)
                    accum += (i*u + (1-i)*(1-u))*
                            (j*v + (1-j)*(1-v))*
                            (k*w + (1-k)*(1-w)) * c[i][j][k];

        return accum;
    }
};

#endif //PERLIN_H
