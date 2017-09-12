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

#include "command.h"
#include "showface.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> // INT_MIN and INT_MAX

void command_help() {
    printf("faces <id> [mindim] [maxdim]\n"
           "    Show id's faces with a dimension of at least "
           "mindim and at most maxdim\n"
           "cofaces <id> [mindim] [maxdim]\n"
           "    Show id's cofaces with a dimension of at least "
           "mindim and at most maxdim\n"
           "betti [n]\n"
           "    Show the Nth betti number, or the first 3 if n is "
           "omitted\n"
           "dimension [id1] [id2] ... [idn]\n"
           "    Show the dimension(s) of some simplices\n"
           "hash\n"
           "    Show the hash table's statistics\n"
           "!<cmd>\n"
           "    Execute a shell command\n"
           "CTRL-D (UNIX) or CTRL-Z + ENTER (DOS)\n"
           "    Quit\n"
           "help, ?\n"
           "    Show this message\n");
}

static void show_hash_statistics(struct scomplex *scomplex) {
    printf("%d buckets\n", scomplex->table_size);

    int occupants = 0;
    int collisions = 0;
    for(int i = 0; i < scomplex->table_size; i++) {
        struct simplex *cur = scomplex->simplices[i];
        if(cur) {
            occupants++;
            cur = cur->next;
        }
        while(cur) {
            collisions++;
            cur = cur->next;
        }
    }

    printf("%d occupants\n", occupants);
    printf("Load factor = %.2f\n",
           occupants / (float)scomplex->table_size);
    printf("%d collisions\n", collisions);
}

static void show_betti(struct scomplex *scomplex, int n) {
    if(n == INT_MAX) {
        printf("%d %d %d\n", scomplex->betti[0], scomplex->betti[1],
               scomplex->betti[2]);
    } else {
        printf("%d\n", scomplex->betti[n]);
    }

    if(scomplex->betti2_unreliable && (n == INT_MAX || n == 2)) {
        fprintf(stderr, "Warning: Betti2 is unreliable\n");
    }
}

static int garbage_at_end() {
    if(strtok(NULL, " \n")) {
        fprintf(stderr, "Error: too many arguments\n");
        return 1;
    }
    return 0;
}

static int get_num(int *num, char **token) {
    char *tok = strtok(NULL, " \n");
    char *unconverted = NULL;
    if(tok) {
        *num = strtol(tok, &unconverted, 10);
        if(unconverted && *unconverted) {
            fprintf(stderr, "%s is not a number\n", tok);
            if(token) *token = tok;
            return 1;
        }
    }
    if(token) *token = tok;
    return 0;
}

void do_command(struct scomplex *scomplex, char *cmd) {
    if(*cmd == '!') {
        // TODO: Redirection (>) doesn't work, it just creates
        // an empty file!
        system(cmd + 1);
        return;
    }

    char *token = strtok(cmd, " \n");
    if(!token || !*token) return;

    if(!strcmp(token, "faces") || !strcmp(token, "cofaces")) {
        const int faces = !strcmp(token, "faces");

        int min = INT_MIN;
        int max = INT_MAX;
        char *id = strtok(NULL, " \n");
        if(!id) {
            fprintf(stderr, "Missing id\n"); return;
        }

        if(get_num(&min, NULL)) return;
        if(get_num(&max, NULL)) return;

        if(min > max) {
            printf("The minimum of %d cannot be bigger than "
                   "the maximum of %d\n", min, max);
        } else if(!garbage_at_end()) {
            if(faces) show_faces(scomplex, id, min, max);
            else show_cofaces(scomplex, id, min, max);
        }
    } else if(!strcmp(token, "help") || !strcmp(token, "?")) {
        if(!garbage_at_end()) command_help();
    } else if(!strcmp(token, "hash")) {
        if(!garbage_at_end()) show_hash_statistics(scomplex);
    } else if(!strcmp(token, "betti")) {
        int n;
        if(get_num(&n, &token)) return;
        if(token && (n < 0 || n > 2)) {
            fprintf(stderr, "Only Betti0 through Betti2 are "
                    "supported\n");
            return;
        }
        if(garbage_at_end()) return;
        show_betti(scomplex, token ? n : INT_MAX);
    } else if(!strcmp(token, "dimension")) {
        while((token = strtok(NULL, " \n"))) {
            struct simplex *s = get_simplex(scomplex, token);
            if(s) printf("%d\n", DIMENSION(s));
            else fprintf(stderr, "No simplex named '%s'\n", token);
        }
    } else {
        fprintf(stderr, "Unknown command '%s', type '?' for "
                "help\n", token);
    }
}

