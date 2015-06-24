// randomsample.c
// an external for Pure Data
// returns a randomized sample of input list
// based on python's random.sample() function
// Jeremy Muller ©2015

#include "m_pd.h"
#include <stdlib.h>
#include <stdbool.h>

static t_class *randomsample_class;

typedef struct _randomsample {
    t_object x_obj; // needed and must be listed first in struct
    t_float f_Nsamples; // !!!!! convert to int before using !!!!!
} t_randomsample;

void setUniqueRandomNums(int *indexes, int sampSize, int popSize)
{
    // find randomized unique indexes
    for (int i = 0; i < sampSize; i++) {

        int f;
        bool exists = true;

        while (exists) {
            exists = false;

            f = (int)(abs(arc4random()) % popSize);
            for (int j = 0; j < sampSize; j++) {
                if (f == indexes[j]) {
                    exists = true;
                    break;
                }
            }
        }
        indexes[i] = f;
    }
}

void listOfNumbers(t_randomsample *x, t_symbol *s, int argc, t_atom *argv)
{
    // further checking is require in case sample size is larger than population
    if (x->f_Nsamples > argc || x->f_Nsamples < 1) {
        post("------\n[randomsample]\nError: incorrect sample size");
        post("Setting argument to size: %i\n------", argc);
        x->f_Nsamples = argc;
    }

    // store samples in this array
    int n = (int)x->f_Nsamples;
    t_float samples[n];

    // store random index
    int indexes[n];
    setUniqueRandomNums(indexes, n, argc);

    t_atom a[n];
    for (int i = 0; i < n; i++) {
        int index = indexes[i];
        samples[i] = atom_getfloatarg(index, argc, argv);
        SETFLOAT(a+i, samples[i]);
    }

    outlet_list(x->x_obj.ob_outlet, s, n, a);
}

void listOfSymbols(t_randomsample *x, t_symbol *s, int argc, t_atom *argv)
{
    // further checking is require in case sample size is larger than population
    if (x->f_Nsamples > argc || x->f_Nsamples < 1) {
        post("------\n[randomsample]\nError: incorrect sample size");
        post("Setting argument to size: %i\n------", argc);
        x->f_Nsamples = argc;
    }

    // store samples in this array
    int n = (int)x->f_Nsamples;
    t_symbol samples[n];

    // store random index
    int indexes[n];
    setUniqueRandomNums(indexes, n, argc);

    t_atom a[n];
    for (int i = 0; i < n; i++) {
        int index = indexes[i];
        samples[i] = *atom_getsymbol(argv+index);
        SETSYMBOL(a+i, &samples[i]);
    }

    outlet_list(x->x_obj.ob_outlet, s, n, a);
}

// constructor
void *randomsample_new(t_floatarg f)
{
    t_randomsample *x = (t_randomsample *)pd_new(randomsample_class);


    if (f < 1) {
        post("[randomsample]: Argument must be greater than 0!");
        f = 1;
    }
    x->f_Nsamples = f;

    floatinlet_new(&x->x_obj, &x->f_Nsamples);

    outlet_new(&x->x_obj, &s_list);
    return (void *)x;
}

// setup routine
void randomsample_setup(void)
{
    post("randomsample, outputs a randomized list from the given input list of length N, version 0.1");
    post("by Jeremy Muller, 2015\n");

    randomsample_class = class_new(gensym("randomsample"),
        (t_newmethod)randomsample_new, 0,
        sizeof(t_randomsample),
        CLASS_DEFAULT,
        A_DEFFLOAT, 0);

    class_addlist(randomsample_class, listOfNumbers);
    //class_addlist(randomsample_class, listOfSymbols);
    class_addmethod(randomsample_class,
        (t_method)listOfSymbols,
        gensym("symbols"),
        A_GIMME, 0);
    class_sethelpsymbol(randomsample_class, gensym("help-randomsample"));
}
