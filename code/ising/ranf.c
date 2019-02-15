/*  -- translated by f2c (version 19970805).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Common Block Declarations */

struct {
    shortint inseed[3];
} seeds_;

#define seeds_1 seeds_

doublereal ranf_()
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal erand48();

    ret_val = erand48(seeds_1.inseed);
    return ret_val;
} /* ranf_ */




/* Subroutine */ int ranset_(jseed)
shortint *jseed;
{
    static integer i__;

    /* Parameter adjustments */
    --jseed;

    /* Function Body */
    for (i__ = 1; i__ <= 3; ++i__) {
	seeds_1.inseed[i__ - 1] = jseed[i__];
    }
    return 0;
} /* ranset_ */




/* Subroutine */ int ranget_(jseed)
shortint *jseed;
{
    static integer i__;

    /* Parameter adjustments */
    --jseed;

    /* Function Body */
    for (i__ = 1; i__ <= 3; ++i__) {
	jseed[i__] = seeds_1.inseed[i__ - 1];
    }
    return 0;
} /* ranget_ */

