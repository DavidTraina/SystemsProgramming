#include <stdio.h>
int main() {
	char phone[11]; // need room for 0 terminator
	printf("Type in a 10 character string\n");
	scanf("%10s", phone);

	int input_int;
	printf("Type in an integer\n");
	scanf("%d", &input_int);
	if (input_int < -1 || input_int > 9) {
		printf("ERROR\n");
		return 1;
	} else if (input_int == -1) {
		printf("%s\n", phone);
	} else {
		printf("%c\n", phone[input_int]);
	}
	return 0;
}