/*
 * This file is part of arm-sim: http://madscientistroom.org/arm-sim
 *
 * Copyright (c) 2010 Randy Thelen. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

#include "forth.h"
#include "fobj.h"

void farray_print(fenv_t *f, fobj_t *p)
{
    ASSERT(p->type == FOBJ_TABLE);
    farray_t *a = &p->u.array;

    for (int i = 0; i < a->num; i++) {
        printf("array[%d] = ", i);
        fobj_print(f, a->elems[i]);
    }

}

fobj_t *farray_new(fenv_t *f)
{
    fobj_t *a = fobj_new(f, FOBJ_ARRAY);
    a->u.array.num = 0;
    a->u.array.elems = NULL;
    return a;
}

void farray_free(fenv_t *f, fobj_t *a)
{
    for (int i = 0; i < a->u.array.num; i++) {
        fobj_release(f, a->u.array.elems[i]);
    }

    if (a->u.array.num > 0) {
        free(a->u.array.elems);
    }
}


static void farray_grow(fenv_t *f, farray_t *a, int n)
{
    ASSERT(n > a->num);
    a->elems = realloc(a->elems, n * sizeof(fobj_t **));
    bzero(&a->elems[a->num], (n - a->num) * sizeof(fobj_t **));
    a->num = n;
}

static fobj_t **farray_num_index(fenv_t *f, farray_t *a, fnumber_t n)
{
    if (n < 0) return NULL;
    if (n >= a->num) return NULL;

    return a->elems + (int) n;
}

static fobj_t *farray_obj_fetch(fenv_t *f, fobj_t **p)
{
    if (!p) {
        return NULL;
    }

    return fobj_retain(f, *p);
}

static void farray_obj_store(fenv_t *f, fobj_t **valp, fobj_t *data)
{
    ASSERT(valp);

    if (*valp) {
        fobj_release(f, *valp);
    }

    *valp = fobj_retain(f, data);
}

void farray_store(fenv_t *f, fobj_t *addr, fobj_t *index, fobj_t *data)
{
    fassert(f, index == NULL, 1, "array must be indexed by NUM");
    fassert(f, index->type == FOBJ_NUM, 1, "array must be indexed by NUM");

    farray_t *a = &addr->u.array;
    fnumber_t n = index->u.num.n;

    fobj_t **valp = farray_num_index(f, a, n);

    if (!valp) {
        farray_grow(f, a, n+1);
        valp = farray_num_index(f, a, n);
        ASSERT(valp);
    }

    farray_obj_store(f, valp, data);
}

fobj_t *farray_fetch(fenv_t *f, fobj_t *addr, fobj_t *index)
{
    fassert(f, index == NULL, 1, "array must be indexed by NUM");
    fassert(f, index->type == FOBJ_NUM, 1, "array must be indexed by NUM");

    farray_t *a = &addr->u.array;
    fnumber_t n = index->u.num.n;

    return farray_obj_fetch(f, farray_num_index(f, a, n));
}

