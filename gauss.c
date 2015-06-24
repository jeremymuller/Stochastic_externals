// guass.c
// an external for Pure Data
// returns a number using gaussian distribution based on mean and variance
// based on python's random.gauss() function
// Jeremy Muller ©2015

#include "m_pd.h"
#include <stdlib.h>
#include <math.h>

// object struct
typedef struct _gauss {
    t_object x_obj; // needed and must be listed first in struct
    t_float mean;
    t_float stddev;

} t_gauss;

// prototypes
static t_class *gauss_class;
void bangGenerateNum(t_gauss *x);
double rand_gauss(double mean, double stddev);

// constructor
void *gauss_new(t_floatarg mean, t_floatarg stddev)
{
    t_gauss *x = (t_gauss *)pd_new(gauss_class);

    x->mean = mean;
    x->stddev = stddev;

    floatinlet_new(&x->x_obj, &x->mean);
    floatinlet_new(&x->x_obj, &x->stddev);

    outlet_new(&x->x_obj, &s_float);
    return (void *)x;
}

// setup routine
void gauss_setup(void)
{
    post("gauss, outputs a number based on gaussian distribution, version 0.1");
    post("by Jeremy Muller, 2015\n");

    gauss_class = class_new(gensym("gauss"),
        (t_newmethod)gauss_new, 0,
        sizeof(t_gauss),
        CLASS_DEFAULT,
        A_DEFFLOAT,
        A_DEFFLOAT, 0);

    class_addbang(gauss_class, bangGenerateNum);
    class_sethelpsymbol(gauss_class, gensym("help-gauss"));
}

// handles bang from inlet
void bangGenerateNum(t_gauss *x)
{
    t_float num = rand_gauss(x->mean, x->stddev);
    outlet_float(x->x_obj.ob_outlet, num);
}

// function that computes gaussian distribution
double rand_gauss(double mean, double stddev)
{
    // this math taken from:
    // http://en.literateprograms.org/Box-Muller_transform_(C)#chunk%20def:complete%20Box-Muller%20function

    static double n2 = 0.0;
    static int n2_cached = 0;

    if (!n2_cached) {
        double x, y, r;
        do
        {
            x = 2.0 * rand() / RAND_MAX - 1;
            y = 2.0 * rand() / RAND_MAX - 1;
            r = x*x + y*y;
        } while (r == 0.0 || r > 1.0);

        double d = sqrt(-2.0 * log(r)/r);
        double n1 = x*d;
        n2 = y*d;

        double result = n1 * stddev + mean;
        n2_cached = 1;
        return result;
    } else {
        n2_cached = 0;
        return n2 * stddev + mean;
    }
}
