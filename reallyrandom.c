// reallyrandom.c
// an external for Pure Data
// uses arc4random() to make a better random
// Jeremy Muller ©2015

#include "m_pd.h"
#include <stdlib.h>

static t_class *reallyrandom_class;

typedef struct _reallyrandom {
    t_object x_obj; // needed and must be listed first in struct
    t_float f_max;
} t_reallyrandom;

// bang method in the left inlet
void reallyrandom_bang(t_reallyrandom *x)
{
    t_float f = 0;

    if (x->f_max >= 1) {
        t_int max = (t_int)x->f_max; // convert to int first
        f = abs(arc4random()) % max;
    }

    outlet_float(x->x_obj.ob_outlet, f);
}

// constructor
void *reallyrandom_new(t_floatarg f)
{
    t_reallyrandom *x = (t_reallyrandom *)pd_new(reallyrandom_class);
    x->f_max = f;

    floatinlet_new(&x->x_obj, &x->f_max);
    outlet_new(&x->x_obj, &s_float);
    return (void *)x;
}

// setup routine
void reallyrandom_setup(void)
{
    post("reallyrandom, a slightly better random number generator, version 0.1");
    post("by Jeremy Muller, 2015\n");

    reallyrandom_class = class_new(gensym("reallyrandom"),
        (t_newmethod)reallyrandom_new,
        0,
        sizeof(t_reallyrandom),
        CLASS_DEFAULT,
        A_DEFFLOAT, 0);

    class_addbang(reallyrandom_class, reallyrandom_bang);
    class_sethelpsymbol(reallyrandom_class, gensym("help-reallyrandom"));
}
