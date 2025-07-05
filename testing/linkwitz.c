#include "R.h"
#include "../include/linkwitz.h"

void calculate_linkwitz_biquad_wrapper(
        double *a, double *b,
        double *f0, double *q0,
        double *fp, double *qp,
        double *fs) {
    calculate_linkwitz_biquad(a, b, *f0, *q0, *fp, *qp, *fs);
}
