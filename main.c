/**
* This program (Faces), is an interactive program that calculates
* the first three Betti numbers.
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
#include "command.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static void usage(FILE *f) {
    fprintf(f, "Usage: faces <file>\n\n"
               "Each line of the file (max 127 chars) is formatted "
               "as follows:\n"
               "<id> <face1> <face2> ... <facen>\n"
               "\nExamples:\n"
               "'v0' declares a vertex named v0\n"
               "'foo bar quux' declares an edge whose vertices "
               "are bar and quux\n"
               "'f1 e0 e1 e2' is a face whose edges are "
               "e0, e1, and e2\n"
               "\n"
               "A line beginning with '#' is a comment.\n"
               "End of line comments are NOT supported.\n");
}

int main(int argc, char **argv) {
    printf("Faces: Copyright 2017 Seth Simon (s.r.simon@csuohio.edu)\n"
           "This program comes with ABSOLUTELY NO WARRANTY; for "
           "details, see the license.\nThis is free software, and "
           "you are welcome to redistribute it\nunder certain "
           "conditions; "
           "see the license for details.\nYou should have received "
           "a copy "
           "of the GNU General Public License\n(version 3) along "
           "with this program. If not, see "
           "<http://www.gnu.org/licenses/>.\n\n");

    if(argc != 2) {
        usage(stderr);
        return 1;
    } else if(!strcmp(argv[1], "?") || !strcmp(argv[1], "-h") ||
              !strcmp(argv[1], "--help") || !strcmp(argv[1], "/?") ||
              !strcmp(argv[1], "/h") || !strcmp(argv[1], "/help")) {
        usage(stdout);
        return 0;
    }

    int ret = 1;
    struct stat st;
    FILE *file;
    if(stat(argv[1], &st) || !(file = fopen(argv[1], "r"))) {
        fprintf(stderr, "Failed to open '%s' for reading\n", argv[1]);
        return 1;
    }
    struct scomplex scomplex = SCOMPLEX_DEFAULTS;
    if(init_scomplex(&scomplex, st.st_size)) goto done;

    char line[128];
    for(int lineno = 1; fgets(line, 128, file); lineno++) {
        if(process_line(&scomplex, line, lineno)) goto done;
    }
    get_betti(scomplex.betti, &scomplex.betti2_unreliable);
    fclose(file);
    file = NULL;

    printf("Type ? for help, CTRL-D (UNIX) or CTRL-Z + ENTER (DOS) "
           "to quit.\n\n? ");
    char cmd[128];
    while(fgets(cmd, 128, stdin)) {
        do_command(&scomplex, cmd);
        printf("\n? ");
    }

    ret = 0;
done:
    if(file) fclose(file);
    free_scomplex(&scomplex);
    return ret;
}

