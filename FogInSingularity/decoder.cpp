#include "stdio.h"

int main() {
	FILE* inp = fopen("original_prog.com", "rb");
	FILE* out = fopen("decoded.com", "wb");

	for (int i = 0; i < 0x0e; i++) {
		putc(0x90, out);
		getc(inp);
	}

	for (int i = 0x0e; i <= 0x12a; i++) {
		putc(~getc(inp), out);
	}

	fclose(inp);
	fclose(out);
}
