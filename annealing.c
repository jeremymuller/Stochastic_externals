// annealing.c
// an external for Pure Data
// uses the simulated annealing algorithm
// Jeremy Muller ©2015

#include "m_pd.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define DEFAULT_SCALING 100
#define NEIGHBOR_RANGE 500
#define RANGE_SIZE NEIGHBOR_RANGE*2

static t_class *annealing_class;
static int range[RANGE_SIZE]; // might not use this

typedef struct _annealing {
    t_object x_obj; // needed and must be listed first in struct
    t_symbol *arrayname;
    t_float scalingFactor, temp, parent, bestSol;
    t_int pIndex;
    t_outlet *parentOut, *bestOut;
} t_annealing;

/**************** PROTOTYPES ****************/
void annealing_bang(t_annealing *x);
void clearIndex(t_annealing *x);
int getNewIndex(int currentIndex, int arrayLength);

// constructor
void *annealing_new(t_symbol *s, t_float f, t_float scale)
{
    srand(time(NULL)); // seed the random number generator only once
    
    t_annealing *x = (t_annealing *)pd_new(annealing_class);
    
    x->arrayname = s;
    
    if (f > 0)
        x->temp = f;
    else
        x->temp = 1;
    
    if (scale > 0)
        x->scalingFactor = scale;
    else
        x->scalingFactor = DEFAULT_SCALING;
    
    x->pIndex = -1;
    
    // inlets & outlets
    floatinlet_new(&x->x_obj, &x->temp);
    x->parentOut = outlet_new(&x->x_obj, &s_float);
    x->bestOut = outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;
}

// setup routine
void annealing_setup(void)
{
    post("annealing, uses the simulated annealing algorithm, version 0.1");
    post("by Jeremy Muller, 2015\n");
    
    // setup array range omitting the 0th element
    for (int i = 0; i < NEIGHBOR_RANGE; i++)
        range[i] = -(NEIGHBOR_RANGE-i);
    for (int i = NEIGHBOR_RANGE; i < NEIGHBOR_RANGE*2; i++)
        range[i] = i + 1 - NEIGHBOR_RANGE;
    
    annealing_class = class_new(gensym("annealing"),
                                (t_newmethod)annealing_new, 0,
                                sizeof(t_annealing),
                                CLASS_DEFAULT,
                                A_DEFSYMBOL,
                                A_DEFFLOAT,
                                A_DEFFLOAT, 0);
    
    class_addbang(annealing_class, annealing_bang);
    class_addmethod(annealing_class, (t_method)clearIndex, gensym("clear"), 0);
    class_addmethod(annealing_class, (t_method)clearIndex, gensym("reset"), 0);
    class_sethelpsymbol(annealing_class, gensym("help-annealing"));
}

void annealing_bang(t_annealing *x)
{
    t_garray *a;
    int npoints;
    t_word *vec;
    
    if (!(a = (t_garray *)pd_findbyclass(x->arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->arrayname->s_name);
    else if (!garray_getfloatwords(a, &npoints, &vec))
        pd_error(x, "%s: bad template for annealing", x->arrayname->s_name);
    else
    {
        if (x->pIndex == -1)
            x->pIndex = (int)(arc4random() % npoints);
        if (x->pIndex < 0)
            x->pIndex = 0;
        else if (x->pIndex >= npoints)
            x->pIndex = npoints - 1;
        
        x->parent = vec[x->pIndex].w_float;
        
        int cIndex = (int)(arc4random() % npoints); // this uses random index
        
        // this uses a random index surrounding the current index
        // int cIndex = getNewIndex(x->pIndex, npoints);
        float child = vec[cIndex].w_float;
        float diff = child - x->parent;
        if (diff > 0) {
            x->pIndex = cIndex;
            x->parent = child;
        } else {
            if (x->temp < 0)
                x->temp = 1;
            float p = pow(M_E, ((diff * x->scalingFactor / x->temp)));
            // post("Difference is: %f. P is = %f", diff, p);
            
            float r = (rand() % 100) / 100.0;
            // float r = (arc4random() % 1000) / 1000.0;
            if (p > r) {
                // post("The switch to a lower solution happened. R is = %f", r);
                x->pIndex = cIndex;
                x->parent = child;
            }
        }
        
        // keep the best solution in case parent leaves it
        if (x->parent > x->bestSol) x->bestSol = x->parent;
        
        // post("PARENT: %f", x->parent);
        // post("BEST SOLUTION: %f", x->bestSol);
        
        outlet_float(x->parentOut, (npoints ? x->parent : 0));
        outlet_float(x->bestOut, x->bestSol);
    }
}

/**************** HELPERS ****************/
void clearIndex(t_annealing *x)
{
    post("cleared");
    x->pIndex = -1;
    x->bestSol = -RAND_MAX;
}

int getNewIndex(int currentIndex, int arrayLength)
{
    // 	Randomly chooses an index left or right of currentIndex that is 0 <= x < arrayLength
    
    int i = arc4random() % RANGE_SIZE;
    int index = range[i];
    post("random index from array: %i", index);
    
    if ((currentIndex+index) > (arrayLength-1) || (currentIndex+index) < 0)
        return currentIndex - index;
    
    return index + currentIndex;
}
