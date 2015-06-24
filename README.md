# Stochastic_externals
These are externals to be compiled and used with the Pure Data visual programming language. 

You will need to download Pure Data https://puredata.info/downloads to use these externals.

Below is a list of these objects that I created with a little explanation about them. These use different ideas from probability theory and some of these were inspired from the Python language’s “random” library, however, these are all written in C. 

> annealing – Uses the simulated annealing algorithm by using an array table lookup. States are simplified by comparing floats from the array and giving their output as you step through the algorithm.

> gauss – Outputs a Gaussian distribution. Might be cool to try some type of granular synthesis using this object (hint hint)…

> randomsample – Takes a list of numbers and outputs a portion of them randomly. Could be useful for generating chords or sequences of notes from a larger population of notes.

> reallyrandom – This just uses a different random number generated (different from Pd’s built-in object) that is a little better. It should be seeded differently each time you open Pd.

> weightedlist – This takes two lists: one that contains the elements and the other that contains the probabilities corresponding to each element. This object is extremely useful for quickly and easily implementing Markov chains.
