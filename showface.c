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

#include "showface.h"

#include <stdio.h>
#include <string.h>

#define PRINT_HEADER printf("Simplex    Dimension\n" \
                            "====================\n")
#define PRINT_SIMPLEX(simp) printf("%-10s %d\n", simp->id,\
                            DIMENSION(simp))

static void show_faces_internal(struct simplex *simp, int dim) {
    if(DIMENSION(simp) == dim) {
        if(!simp->processed) {
            simp->processed = 1;
            PRINT_SIMPLEX(simp);
        }
        return;
    }
    for(int idx = 0; idx < simp->nfaces; idx++) {
        show_faces_internal(simp->faces[idx], dim);
    }
}

void show_faces(struct scomplex *scomplex, char *id, int mindim,
                int maxdim) {
    struct simplex *simp = get_simplex(scomplex, id);
    if(!simp) {
        fprintf(stderr, "No simplices have id '%s'\n", id);
        return;
    }
    reset_processed_flags(simp, 0);
    PRINT_HEADER;

    if(maxdim > DIMENSION(simp)) maxdim = DIMENSION(simp);
    if(mindim < 0) mindim = 0;
    for(int i = mindim; i <= maxdim; i++) {
        show_faces_internal(simp, i);
    }
}

static void show_cofaces_internal(struct simplex *simp, int dim) {
    if(DIMENSION(simp) == dim) {
        if(!simp->processed) {
            simp->processed = 1;
            PRINT_SIMPLEX(simp);
        }
        return;
    }
    for(int idx = 0; idx < simp->ncofaces; idx++) {
        show_cofaces_internal(simp->cofaces[idx], dim);
    }
}

void show_cofaces(struct scomplex *scomplex, char *id,
                  int mindim, int maxdim) {
    struct simplex *simp = get_simplex(scomplex, id);
    if(!simp) {
        fprintf(stderr, "No simplices have id '%s'\n", id);
        return;
    }
    reset_processed_flags(simp, 1);
    PRINT_HEADER;

    if(mindim < DIMENSION(simp)) mindim = DIMENSION(simp);
    if(maxdim > scomplex->max_dim) maxdim = scomplex->max_dim;
    for(int i = mindim; i <= maxdim; i++) {
        show_cofaces_internal(simp, i);
    }
}

