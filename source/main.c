#include <stdio.h>
#include "tokenizer.h"
// #include <dlfcn.h>

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("Usage: %s [filepath]\n", argv[0]);
		return 1;
	}

	// void * handle = dlopen("libclang.dylib", RTLD_LAZY);
	// if (!handle) {
	// 	printf("Could not load libclang\n");
	// 	return 0;
	// }
	// return 0;

	const char * filepath = argv[1];

	TLTokenizer tk = logos_createTokenizer(filepath);
	// logos_dumpTokens(tk, 1);
	logos_analyseTokens(tk);
	logos_disposeTokenizer(tk);

	return 0;
}
