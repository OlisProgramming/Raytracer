#pragma once

#include <stdexcept>
#include <string>

// The caller is responsible for delete[]ing the result of this function.
char* readFile(const char *fname) {
	FILE *f = fopen(fname, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	rewind(f);

	char *string = new char[fsize + 1];
	fread(string, fsize, 1, f);
	fclose(f);

	string[fsize] = 0;
	return string;
}