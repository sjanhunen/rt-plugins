#ifndef LINKWITZ_H
#define LINKWITZ_H

#include <math.h>

#ifndef BIQUAD_TYPE
#define BIQUAD_TYPE double
#endif

typedef BIQUAD_TYPE bq_t;

/* Linkwitz transform biquad calculator
   Calculates coefficients for a Linkwitz transform that converts
   a speaker with f0/Q0 to a target response of fp/Qp */
static inline void calculate_linkwitz_biquad(
        bq_t *a, bq_t *b,
        bq_t f0, bq_t q0,
        bq_t fp, bq_t qp,
        bq_t fs) {

    // Analog domain coefficients
    bq_t d0i = pow((2*M_PI*f0), 2);
    bq_t d1i = (2*M_PI*f0)/q0;
    bq_t d2i = 1;

    bq_t c0i = pow((2*M_PI*fp), 2);
    bq_t c1i = (2*M_PI*fp)/qp;
    bq_t c2i = 1;

    bq_t fc = (f0 + fp) / 2;
    bq_t gn = (2*M_PI*fc)/(tan(M_PI*fc/fs));
    bq_t cci = c0i+gn*c1i+gn*gn*c2i;

    // Digital domain coefficients
    a[0] = 1;
    a[1] = (2*(c0i-(gn*gn)*c2i)/cci);
    a[2] = ((c0i-gn*c1i+(gn*gn)*c2i)/cci);

    b[0] = (d0i+gn*d1i+(gn*gn)*d2i)/cci;
    b[1] = 2*(d0i-(gn*gn)*d2i)/cci;
    b[2] = (d0i-gn*d1i+(gn*gn)*d2i)/cci;
}

#endif // LINKWITZ_H 