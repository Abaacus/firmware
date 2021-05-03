#include "fake_main.h"
#include <stdio.h>
#include "unity.h"
#define ERROR_STRING_MAX 50
static const char * error_format_str = "ERROR: File: %s, Line: %d";
void fake_Error_Handler(char *file, int line, int cmock_num_calls) {
	char output_str[ERROR_STRING_MAX] = {0};
	snprintf(output_str, ERROR_STRING_MAX, error_format_str, file, line);
	TEST_FAIL_MESSAGE(output_str);
}
