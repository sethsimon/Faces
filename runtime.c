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

#include <stdio.h>
#include <stdlib.h>

/**
 * runtime.c
 *
 * Creates two test files for analyzing the running time
 * of the algorithm in different scenarios.
*/

int main(int argc, char **argv) {
    if(argc != 4) {
        printf("Usage: runtime.exe <# vertices> <left-to-right-file> "
               "<right-to-left-file>\n");
        return 1;
    }

    char *unconverted = NULL;
    const int nvertices = (int)strtol(argv[1], &unconverted, 10);
    if((unconverted && *unconverted) || nvertices <= 0) {
        printf("Invalid vertex count\n");
        return 1;
    }

    FILE *ltr = fopen(argv[2], "w");
    FILE *rtl = fopen(argv[3], "w");
    if(!ltr || !rtl) {
        printf("Failed to open output file\n");
        if(rtl) fclose(rtl);
        return 1;
    }

    fprintf(ltr, "# This is the left-to-right file created by "
            "runtime.c\n");
    fprintf(rtl, "# This is the right-to-left file created by "
            "runtime.c\n");

    for(int i = 0; i < nvertices; i++) {
        fprintf(ltr, "v%d\n", i);
        fprintf(rtl, "v%d\n", i);
    }

    for(int i = 0; i < nvertices - 1; i++) {
        fprintf(ltr, "e%d v%d v%d\n", i, i, i + 1);
        fprintf(rtl, "e%d v%d v%d\n", i, nvertices - 1 - i,
                nvertices - 2 - i);
    }

    fclose(rtl);
    fclose(ltr);
    return 0;
}

