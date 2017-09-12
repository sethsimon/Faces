/**
* This file is part of Faces.
* Copyright (C) 2017 Seth Simon (s.r.simon@csuohio.edu)
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "simplex.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define FACES_AT_ONCE 4
#define PTRSIZE (sizeof(void *))

void free_simplex(struct simplex *simp) {
    free(simp->faces);
    free(simp->cofaces);
    free(simp->id);
    free(simp);
}

int insert_face(struct simplex *simp, struct simplex *face) {
    if(!simp->faces) {
        simp->faces = malloc(FACES_AT_ONCE * PTRSIZE);
        if(!simp->faces) return 1;
    } else if(simp->nfaces % FACES_AT_ONCE == 0) {
        void *tmp = realloc(simp->faces, PTRSIZE * simp->nfaces * 2);
        if(!tmp) return 1;
        simp->faces = tmp;
    }
    simp->faces[simp->nfaces++] = face;
    return 0;
}

int insert_coface(struct simplex *simp, struct simplex *coface) {
    if(!simp->cofaces) {
        simp->cofaces = malloc(FACES_AT_ONCE * PTRSIZE);
        if(!simp->cofaces) return 1;
    } else if(simp->ncofaces % FACES_AT_ONCE == 0) {
        void *tmp =
            realloc(simp->cofaces, PTRSIZE * simp->ncofaces * 2);
        if(!tmp) return 1;
        simp->cofaces = tmp;
    }
    simp->cofaces[simp->ncofaces++] = coface;
    return 0;
}

void reset_processed_flags(struct simplex *simp, const int co) {
    simp->processed = 0;
    const int count = co ? simp->ncofaces : simp->nfaces;
    struct simplex **arr = co ? simp->cofaces : simp->faces;
    for(int i = 0; i < count; i++) {
        reset_processed_flags(arr[i], co);
    }
}

// ====================
//      Betti
// ====================

/**
 * Sets all of the n-simplices connected to simp via an
 * (n+1)-simplex to val (where n == DIMENSION(simp))
*/
static void set_id(struct simplex *simp, const unsigned val) {
    simp->processed = val;
    for(int coidx = 0; coidx < simp->ncofaces; coidx++) {
        struct simplex *cosimp = simp->cofaces[coidx];
        for(int faidx = 0; faidx < cosimp->nfaces; faidx++) {
            if(cosimp->faces[faidx]->processed != val) {
                set_id(cosimp->faces[faidx], val);
            }
        }
    }
}

static int betti[3] = {0, 0, 0};
static int betti2_unreliable = 0;
static unsigned next_free_id = 0;

static int faces_have_same_id(const struct simplex *simp) {
    if(!simp->nfaces) return 1;

    const unsigned id = simp->faces[0]->processed;
    for(int i = 1; i < simp->nfaces; i++) {
        if(simp->faces[i]->processed != id) return 0;
    }
    return 1;
}

static unsigned min_face_id(const struct simplex *simp) {
    unsigned ret = UINT_MAX;
    for(int i = 0; i < simp->nfaces; i++) {
        if(simp->faces[i]->processed < ret) {
            ret = simp->faces[i]->processed;
        }
    }
    return ret;
}

// TODO: Don't use processed because of [add]/[remove] commands
void add_betti(struct simplex *simp) {
    if(DIMENSION(simp) > 2) {
        betti2_unreliable = 1;
        return;
    }

    if(faces_have_same_id(simp)) {
        betti[DIMENSION(simp)]++;
    } else {
        betti[DIMENSION(simp) - 1]--;
        const unsigned min = min_face_id(simp);
        for(int i = 0; i < simp->nfaces; i++) {
            if(simp->faces[i]->processed != min) {
                set_id(simp->faces[i], min);
            }
        }
    }
    simp->processed = next_free_id++;
}

void get_betti(int *b, int *unreliable) {
    b[0] = betti[0];
    b[1] = betti[1];
    b[2] = betti[2];
    *unreliable = betti2_unreliable;
}

