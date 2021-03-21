#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "helper.h"
#include <sys/wait.h>

// void read_and_print(FILE* infp) {
//     struct rec record;
//     rewind(infp);
//     while(fread(&record, sizeof(struct rec), 1, infp) > 0) {
//         printf("%s-%d\n",record.word, record.freq);
//     }
//     printf("done\n");
// }

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
        exit(1);
    }

    int num_processes;
    char *in_file, *out_file;
    char ch;
    while ((ch = getopt(argc, argv, "n:f:o:")) != -1) {
        switch(ch) {
        case 'n':
            num_processes = atoi(optarg);
            break;
        case 'f':
            in_file = optarg;
            break;
        case 'o':
            out_file = optarg;
            break;
        default:
            fprintf(stderr, "Usage: psort -n <number of processes> -f <inputfile> -o <outputfile>\n");
            exit(1);
        }
    }

    // get file size so each child doesn't need to calculate
    FILE *infp;
    if ((infp = fopen(in_file, "r")) == NULL) {
        fprintf(stderr, "Could not open %s\n", in_file);
        exit(1);
    }
    int file_size = get_file_size(in_file);
    if (fclose(infp) == EOF) {
        perror("fclose in_file in parent");
        exit(1);
    }

    // Exit if file is empty or user entered invalid number of processes
    if (file_size == 0 || num_processes < 1) {
        return 0;
    }

    int recs_in_file = file_size / sizeof(struct rec);
    // Let M = the number of recs in the file, N = number of processes. We want to divide M into N groups as evenly as possible.
    // Then M = floor(M/N)*N + (M%N). So, we make N groups of floor(M/N), then add 1 to (M%N) of them.
    int base_sort_amt = recs_in_file / num_processes; // The minimum amount of recs a child will sort (integer division is floor division)
    int num_sort_extra = recs_in_file % num_processes; // The number of children that will sort an extra rec.
    
    if (recs_in_file < num_processes) { // no point in making processes that will sort nothing.
        num_processes = recs_in_file;
    }

    int pipe_fd[num_processes][2];
    
    for (int i = 0; i < num_processes; i++) {
        
        // setup pipe
        if ((pipe(pipe_fd[i])) == -1) {
            perror("pipe");
            exit(1);
        }
        
        // fork child process
        int result = fork();
        if (result < 0) { 
            perror("fork");
            exit(1);
        } else if (result == 0) { // in child            
            
            // close child read end, dont need it
            if (close(pipe_fd[i][0]) == -1) {
                perror("close reading end from inside child failed");
                exit(1);
            }
            
            // Before we forked, the parent had open the reading ends to all previously forked children, now close them here.
            for (int j = 0; j < i; j++) {
                if (close(pipe_fd[j][0]) == -1) {
                    perror("close parent's leftover reading end of previously forked child failed");
                    exit(1);
                }
            }

            if ((infp = fopen(in_file, "r")) == NULL) {
                fprintf(stderr, "Could not open %s\n", in_file);
                exit(1);
            }

            // calculate number of recs to sort and at which index of file to get them
            int num_to_sort = base_sort_amt;
            int start_sorting_i = base_sort_amt * i * sizeof(struct rec);
            if (i < num_sort_extra) {
                num_to_sort++;
                start_sorting_i += i * sizeof(struct rec);
            } else {
                start_sorting_i += num_sort_extra * sizeof(struct rec);
            }
            fseek(infp, start_sorting_i, SEEK_SET);
            struct rec *records = malloc(num_to_sort * sizeof(struct rec)); // stack memory is limited, so we allocate on the heap.
            if (records == NULL) {
                perror("malloc");
                exit(1);
            }
            
            //read the recs we need to sort
            int num_read;
            int num_read_total = 0;
            while ((num_read_total += (num_read = fread(records, sizeof(struct rec), num_to_sort, infp))) != num_to_sort) {
                if (num_read == 0) {
                    perror("fread in child");
                    exit(1);
                }
            }

            if (fclose(infp) == EOF) {
                perror("fclose in_file in parent");
                exit(1);
            }
            
            // sort recs
            qsort(records, num_to_sort, sizeof(struct rec), compare_freq);

            // write sorted recs to pipe in order
            for (int j = 0; j < num_to_sort; j++) {
                if (write(pipe_fd[i][1], &(records[j]), sizeof(struct rec)) != sizeof(struct rec)) {
                    perror("writing to pipe from child");
                    exit(1);
                }
            }

            // Child done writing to the pipe so close it
            if (close(pipe_fd[i][1]) == -1) {
                perror("close pipe in child after writing");
                exit(1);
            }

            // exit so child doesn't continue looping and doesn't have its own children
            free(records);
            exit(0);

        } else { // parent 
            if (close(pipe_fd[i][1]) == -1) { // close write end of pipe in parent or else children will inherit the open fd
                perror("close write end of pipe in parent");
                exit(1);               
            }
        }
    }

    // only parent will get here
    FILE *outfp;
    if ((outfp = fopen(out_file, "w")) == NULL) {
        fprintf(stderr, "Could not open %s\n", out_file);
        exit(1);
    }
    
    // We read from pipe before waiting, no problem because read() will block until child writes or closes its fd for write end of the pipe.
    // If a child terminates abnormally, all its file descriptors are automatically closed so no worries about blocking on read forever.
    // load 1 rec from each pipe into recs_to_compare
    struct rec recs_to_compare[num_processes];
    for (int i = 0; i < num_processes; i++) {
        if (read(pipe_fd[i][0], &recs_to_compare[i], sizeof(struct rec)) == -1) { // read will not be 0 because every child sorted at least 1 rec
            perror("read from pipe");
            exit(1);
        }
    }

    // write all recs to output file
    // When we write recs_to_compare[k] to output and child has closed write end of pip_fd[k], 
    // this index is now invalid because there is nothing more to read there.
    int smallest_valid_i = 0; // smallest index of recs_to_compare that is valid.
    while (smallest_valid_i < num_processes) { // if false, then no valid indexes, so we have written all recs from in_file (or a child terminated abnormally)
        int min_i = smallest_valid_i; // the index of the rec with  minimum frequency
        // find index of minimal rec
        for (int i = smallest_valid_i+1; i < num_processes; i++) {
            if ((recs_to_compare[i]).freq == -1) { // not valid
                continue;
            }
            if ((recs_to_compare[i]).freq < (recs_to_compare[min_i]).freq) {
                min_i = i;
            }
        }
        
        // write minimal rec to output file
        if (fwrite(&(recs_to_compare[min_i]), sizeof(struct rec), 1, outfp) != 1) {
            perror("fwrite");
            exit(1);
        }        
        
        // read a new rec from the pipe corresponding to index of minimal rec we just wrote to file
        int r = read(pipe_fd[min_i][0], &(recs_to_compare[min_i]), sizeof(struct rec));
        if (r != sizeof(struct rec)) {
            if (r == 0) { // we've read everything from the pipe, thus, min_i is now an invalid index
                
                // close the pipe, nothing more to read here
                if (close(pipe_fd[min_i][0]) == -1) {
                    perror("close pipe in parent after reading");
                    exit(1);
                }

                recs_to_compare[min_i].freq = -1; // legal frequencies are non-negative; this is a sentinal value indicating min_i is invalid.
                
                if (min_i == smallest_valid_i) { // min_i is no longer valid so need to find next smallest valid index.
                    smallest_valid_i++;
                    while((smallest_valid_i < num_processes) && ((recs_to_compare[smallest_valid_i]).freq == -1)) { // makes use of short circuiting 
                        smallest_valid_i++;
                    }
                }
            } else {
                perror("read from pipe in parent");
                exit(1);
            }
        } 

    }

    if (fclose(outfp) == EOF) {
        perror("fclose out_file");
        exit(1);
    }

    // collect exit statuses of children
    int status;
    char abnormal_termination = 0;
    for (int i = 0; i < num_processes; i++) {
        if (wait(&status) == -1) {
            fprintf(stderr, "wait\n");
        }
        if (!(WIFEXITED(status))) {
            fprintf(stderr, "Child terminated abnormally\n");
            abnormal_termination = 1;
        }
    }
    if (abnormal_termination) {
        exit(1);
    }

    // FILE *test = fopen(out_file, "r");
    // read_and_print(test);
    // fclose(test);
    return 0;
}
