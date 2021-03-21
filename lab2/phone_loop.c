#include <stdio.h>
int main() {
	int return_code = 0;
	char phone[11]; // need room for 0 terminator
	printf("Type in a 10 character string\n");
	scanf("%10s", phone);

	int input_int;
	while (1) {
	    printf("Type in an integer\n");
		if (scanf("%d", &input_int) == EOF) {
			break;
		}
		if (input_int < -1 || input_int > 9) {
			printf("ERROR\n");
			return_code = 1;
		} else if (input_int == -1) {
			printf("%s\n", phone);
		} else {
			printf("%c\n", phone[input_int]);
		}
	}
	return return_code;
}