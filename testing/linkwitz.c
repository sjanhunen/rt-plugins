#include "R.h"
#include <math.h>

void calculate_linkwitz_biquad(
        double *a, double *b,
        double f0, double q0,
        double fp, double qp,
        double fs) {

    // Analog domain coefficients
    double d0i = pow((2*PI*f0), 2);
    double d1i = (2*PI*f0)/q0;
    double d2i = 1;

    double c0i = pow((2*PI*fp), 2);
    double c1i = (2*PI*fp)/qp;
    double c2i = 1;

    double fc = (f0 + fp) / 2;
    double gn = (2*PI*fc)/(tan(PI*fc/fs));
    double cci = c0i+gn*c1i+gn*gn*c2i;

    // Digital domain coefficients
    a[0] = 1;
    a[1] = (2*(c0i-(gn*gn)*c2i)/cci);
    a[2] = ((c0i-gn*c1i+(gn*gn)*c2i)/cci);

    b[0] = (d0i+gn*d1i+(gn*gn)*d2i)/cci;
    b[1] = 2*(d0i-(gn*gn)*d2i)/cci;
    b[2] = (d0i-gn*d1i+(gn*gn)*d2i)/cci;
}

void calculate_linkwitz_biquad_wrapper(
        double *a, double *b,
        double *f0, double *q0,
        double *fp, double *qp,
        double *fs) {
    calculate_linkwitz_biquad(a, b, *f0, *q0, *fp, *qp, *fs);
}
