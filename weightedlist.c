// weightedlist.c
// an external for Pure Data
// takes a list of weighted probabilities
// Jeremy Muller ©2015

#include "m_pd.h"
#include <stdlib.h>

#define MAX 100

static t_class *weightedlist_class;

typedef struct _weightedlist {
    t_object x_obj; // needed and must be listed first in struct
    t_int i_totalprob;
    t_int capacity;
    t_int listlength;
    t_float *elementlist;
} t_weightedlist;

/**************** PROTOTYPES ****************/
void elements(t_weightedlist *x, t_symbol *s, int argc, t_atom *argv);
void clear(t_weightedlist *x);
void weights(t_weightedlist *x, t_symbol *s, int argc, t_atom *argv);
void allocSpaceForList(t_weightedlist *x, int argc, t_atom *argv);
int selectIndexUsingProbabilities(int max, int index, t_float *arrayOfProbs, int randomNum);

// constructor
void *weightedlist_new(t_symbol *s, int argc, t_atom *argv)
{
    t_weightedlist *x = (t_weightedlist *)pd_new(weightedlist_class);

    allocSpaceForList(x, argc, argv);

    // inlets & outlets
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("list"), gensym("elements"));
    outlet_new(&x->x_obj, &s_float);
    return (void *)x;
}

// destructor
void free_memory(t_weightedlist *x)
{
    if (x->elementlist != NULL) {
        post("freeing memory...");
        free(x->elementlist);
    }
}

// setup routine
void weightedlist_setup(void)
{
    post("weightedlist, outputs an element of list based on probability weights, version 0.2");
    post("by Jeremy Muller, 2015\n");

    weightedlist_class = class_new(gensym("weightedlist"),
                            (t_newmethod)weightedlist_new,
                            (t_method)free_memory,
                            sizeof(t_weightedlist),
                            CLASS_DEFAULT,
                            A_GIMME, 0);

    class_addlist(weightedlist_class, weights);
    class_addmethod(weightedlist_class, (t_method)elements, gensym("elements"), A_GIMME, 0);
    class_addmethod(weightedlist_class, (t_method)clear, gensym("clear"), 0);
    class_sethelpsymbol(weightedlist_class, gensym("help-weightedlist"));
}

void clear(t_weightedlist *x)
{
    free_memory(x);
    x->elementlist = NULL;
    x->listlength = 0;
    x->capacity = 0;
}

// list of elements
void elements(t_weightedlist *x, t_symbol *s, int argc, t_atom *argv)
{
    allocSpaceForList(x, argc, argv);
}

// list of weights in the left inlet
void weights(t_weightedlist *x, t_symbol *s, int argc, t_atom *argv)
{
    t_float outF = 0;
    t_float probabilities[argc];
    t_float max_probs = 0.0;

    for (int i = 0; i < argc; i++)
        max_probs += atom_getfloat(argv+i);

    for (int i = 0; i < argc; i++)
        probabilities[i] = MAX * (atom_getfloat(argv+i)/max_probs);

    // choose random index
    t_int f = (abs(arc4random()) % MAX) + 1;
    int index = selectIndexUsingProbabilities(MAX, argc-1, probabilities, f);

    if (x->listlength > 0) {
        // set index to maximum list item if it's out of bounds
        if (index >= x->listlength)
            index = (int)(x->listlength-1);

        outF = *(x->elementlist + index);
    } else {
        outF = index;
    }

    outlet_float(x->x_obj.ob_outlet, outF);
}


/**************** HELPERS ****************/

void allocSpaceForList(t_weightedlist *x, int argc, t_atom *argv)
{
    // allocate or realloc memory
    if (x->capacity < argc) {
        if (x->elementlist == NULL) {
            x->capacity = argc;
            x->listlength = argc;
            x->elementlist = malloc(argc * sizeof(*x->elementlist));
            if (x->elementlist == NULL)
                error("Unable to allocated memory!!");
        } else {
            x->capacity *= 2;
            x->listlength = argc;
            x->elementlist = realloc(x->elementlist, x->capacity * sizeof(*x->elementlist));
            if (x->elementlist == NULL)
                error("realloc failed");
        }
    } else {
        x->listlength = argc;
    }

    // set array to elementlist
    for (int i = 0; i < x->listlength; i++)
        *(x->elementlist+i) = atom_getfloat(argv+i);
}

int selectIndexUsingProbabilities(int max, int index, t_float *arrayOfProbs, int randomNum)
{
    // This is where all the magic happens

    /*
     * This method uses recursion to check where the random number is based on
     * the given probability of each melody.
     * It starts at 100 and subtracts the probability to give the specific range.
     */

    // post("max: %i, index: %i, randomNum: %i", max, index, randomNum);

    if (max < 1 || index < 1)
        return index;
    else if (randomNum <= max && randomNum > (max - arrayOfProbs[index]))
        return index;
    else
        return selectIndexUsingProbabilities(max-arrayOfProbs[index], index-1, arrayOfProbs, randomNum);
}
