#include <stdio.h>

void print_state(char *state, int size) {
	printf("%.*s\n", size, state); // Specify the precision with size.
}

void update_state(char *state, int size) {
	int prev_changed = 0; // We need to whole process array simultaneously.
	  // We don't want to have to copy the array to be able to modify the
	  // original while keeping a snapshot of what it was, so we use this to 
	  // tell if we should interpret the previous neighbouring cell as it is 
	  // currently (0), or as the inverted version of itself (1).
	for (int i = 1; i < size - 1; i++) { // Omit the first/last elements.
		char change_to;
		if ((state[i-1] == state[i+1]) == prev_changed) { // Opposite values so cell goes to 'X'.
		  // (a == b) returns 0 if false, 1 if true.
	      // if neighbours are equal and prev_changed == 1, then LHS=RHS=1 so evaluates true.
		  // if neighbours are opposite and prev_changed == 0, then LHS=RHS=0, so evaluates true.
			change_to = 'X';
		} else { // Same value on both sides, cell goes to '.'.
			change_to = '.';
		}
		if (state[i] == change_to) {
			prev_changed = 0; // This cell can be interpreted as is when processing next cell.
		} else {
			state[i] = change_to; // This cell needs to interpreted as the inversion because we changed it.
			prev_changed = 1;
		}
	}
}