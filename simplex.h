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

#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <stdio.h>

#define SIMPLEX_DEFAULTS (struct simplex) {\
        .nfaces = 0,\
        .faces = NULL,\
        .ncofaces = 0,\
        .cofaces = NULL,\
        .id = NULL,\
        .next = NULL,\
        .processed = 0\
    }
struct simplex {
    int nfaces;
    struct simplex **faces;

    int ncofaces;
    struct simplex **cofaces;

    char *id;

    struct simplex *next; // for the hash table

    // Preprocessing -> id number (see research paper)
    // Interactive   -> Whether it's been printed or not (showface.c)
    unsigned processed;
};

#define DIMENSION(simp) (((simp)->nfaces) ? ((simp)->nfaces - 1) : 0)

void free_simplex(struct simplex *simp);
int insert_face(struct simplex *simp, struct simplex *face);
int insert_coface(struct simplex *simp, struct simplex *coface);

void reset_processed_flags(struct simplex *simp, const int co);

void add_betti(struct simplex *simp);
void get_betti(int *b, int *unreliable);

#endif

