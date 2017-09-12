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

#ifndef SCOMPLEX_H
#define SCOMPLEX_H

#include "simplex.h"

#include <stdio.h>

#define SCOMPLEX_DEFAULTS (struct scomplex) {\
        .table_size = 0,\
        .simplices = NULL,\
        \
        .max_dim = 0,\
        \
        .betti = { 0, 0, 0 },\
        .betti2_unreliable = 0\
    }
struct scomplex {
    int table_size;
    struct simplex **simplices;

    int max_dim; // used in showface.c

    int betti[3];
    int betti2_unreliable;
};

int init_scomplex(struct scomplex *scomplex, const size_t fsize);
int process_line(struct scomplex *scomplex, char *line,
                 const int lineno);
void free_scomplex(struct scomplex *scomplex);

struct simplex *get_simplex(struct scomplex *scomplex,
                            const char *id);

#endif

