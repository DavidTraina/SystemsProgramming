#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Reads a trace file produced by valgrind and an address marker file produced
 * by the program being traced. Outputs only the memory reference lines in
 * between the two markers
 */

int main(int argc, char **argv) {
    
    if(argc != 3) {
         fprintf(stderr, "Usage: %s tracefile markerfile\n", argv[0]);
         exit(1);
    }

    FILE *marker_file = fopen(argv[2], "r");
    FILE *trace_file = fopen(argv[1], "r");
    if (marker_file == NULL || trace_file == NULL) {
        printf("There was an error opening the file\n");
        return 1;
    }
    // Addresses should be stored in unsigned long variables
    unsigned long start_marker, end_marker;
    fscanf(marker_file, "%lx %lx", &start_marker, &end_marker);
    fclose(marker_file);

    char letter;
    unsigned long hex;
    char line[50];
    char start_printing = 0;
    char character;
    char letter_found;
    int i;
    while (fgets(line, 50, trace_file) != NULL) {
        i = 0;
        letter_found = 0;
        while (i < 50) {
            character = line[i];
            if (character != ' ' && character != '\t') {
                if (letter_found == 0) {
                    letter = line[i];
                    letter_found = 1;
                } else {
                    hex = strtol(&(line[i]), NULL, 16);
                    break;
                }
            }
            i++;
        }
        
        if (hex == end_marker) {
            break;
        }
        if (start_printing == 0) {
            if (hex == start_marker) {
                start_printing = 1;
            }
        } else {
            printf("%c,%#lx\n", letter, hex);
        }
    }
    fclose(trace_file);
    /* For printing output, use this exact formatting string where the
     * first conversion is for the type of memory reference, and the second
     * is the address
     */
    // printf("%c,%#lx\n", VARIABLES TO PRINT GO HERE);
    return 0;
}
