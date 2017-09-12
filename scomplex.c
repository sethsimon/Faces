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

#include "scomplex.h"

#include <stdlib.h>
#include <string.h>

#define PTRSIZE (sizeof(void *))

void free_scomplex(struct scomplex *scomplex) {
    for(int i = 0; i < scomplex->table_size; i++) {
        struct simplex *simp = scomplex->simplices[i];
        while(simp) {
            struct simplex *next = simp->next;
            free_simplex(simp);
            simp = next;
        }
    }
}

static unsigned calc_hash(struct scomplex *scomplex, const char *id) {
    // djb2 hashing algorithm by Dan Bernstein
    // See http://www.cse.yorku.ca/~oz/hash.html
    unsigned ret = 5381;
    for( ; *id; id++) {
        ret = 33 * ret + *id;
    }
    return ret % scomplex->table_size;
}

struct simplex *get_simplex(struct scomplex *scomplex,
                            const char *id) {
    const unsigned hash = calc_hash(scomplex, id);
    struct simplex *ret = scomplex->simplices[hash];

    while(ret && strcmp(ret->id, id)) {
        ret = ret->next;
    }
    return ret;
}

static struct simplex *add_simplex(struct scomplex *scomplex,
                                   const char *id) {
    struct simplex *ret = malloc(sizeof(struct simplex));
    if(!ret) return NULL;
    *ret = SIMPLEX_DEFAULTS;

    ret->id = malloc(strlen(id) + 1);
    if(!ret->id) {
        free(ret);
        return NULL;
    }
    strcpy(ret->id, id);

    const unsigned hash = calc_hash(scomplex, id);
    if(!scomplex->simplices[hash]) {
        scomplex->simplices[hash] = ret;
    } else {
        struct simplex *cur = scomplex->simplices[hash];
        while(cur->next) cur = cur->next;
        cur->next = ret;
    }
    return ret;
}

int init_scomplex(struct scomplex *scomplex, const size_t fsize) {
    // Assume ~10 chars/simplex, aim for load factor = 0.5
    // TODO: grep -v "^#" foo | wc -l would be better
    scomplex->table_size = fsize / 5 + 1;

    scomplex->simplices = malloc(scomplex->table_size * PTRSIZE);
    if(!scomplex->simplices) {
        fprintf(stderr, "Malloc failed in init_scomplex\n");
        return 1;
    }

    memset(scomplex->simplices, 0, scomplex->table_size * PTRSIZE);
    return 0;
}

int process_line(struct scomplex *scomplex, char *line,
                 const int lineno) {
    // \n can be included in the line
    char *token = strtok(line, " \r\t\n");
    if(!token || !*token || *token == '#') return 0;

    if(get_simplex(scomplex, token)) {
        fprintf(stderr, "Line %d: Duplicate id '%s'\n", lineno,
                token);
        return 1;
    }

    struct simplex *const simp = add_simplex(scomplex, token);
    if(!simp) {
        fprintf(stderr, "Line %d: Malloc failed\n", lineno);
        return 1;
    }

    while((token = strtok(NULL, " \r\t\n"))) {
        struct simplex *const face = get_simplex(scomplex, token);
        if(!face) {
            fprintf(stderr, "Line %d: Couldn't find a simplex with "
                    "id '%s'\n", lineno, token);
            return 1;
        }
        if(insert_face(simp, face) || insert_coface(face, simp)) {
            fprintf(stderr, "Line %d: Malloc failed\n", lineno);
            return 1;
        }
    }

    if(simp->nfaces == 1) {
        fprintf(stderr, "Line %d: Malformed face with exactly one "
                "simplex\n", lineno);
        return 1;
    }
    for(int i = 0; i < simp->nfaces; i++) {
        if(DIMENSION(simp->faces[i]) + 1 != DIMENSION(simp)) {
            fprintf(stderr, "Line %d: Since %s has %d faces, %s "
                    "must have dimension %d, not %d\n", lineno,
                    simp->id, simp->nfaces, simp->faces[i]->id,
                    DIMENSION(simp) - 1, DIMENSION(simp->faces[i]));
            return 1;
        }
    }

    if(DIMENSION(simp) > scomplex->max_dim) {
        scomplex->max_dim = DIMENSION(simp);
    }
    add_betti(simp);
    return 0;
}

