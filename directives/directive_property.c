#include "directive.h"

typedef struct Metadata {
	unsigned int index;
	CXToken percentageToken;

	CXToken * * attributeList;
	unsigned int * num_attributeList;
	unsigned int num_attributeCount;

	CXToken * typeNameList;
	unsigned int num_typeNameList;
} Metadata;

static const char logos_directive_name[] = "property";

static void * logos_directive_parse(TLTokenizer tk, CXToken percentageToken) {
	Metadata * metadata = (Metadata *)calloc(1, sizeof(Metadata));
	if (metadata) {
		metadata->percentageToken = percentageToken;

		CXToken token;
		if (logos_peekToken(tk, &token)) {
			if (!logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, "(", NULL)) {
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "expected '('");
				logos_directive_dispose(tk, metadata);
				return NULL;
			}
		}

		TLArgumentsParseEnum r = logos_parseFunctionArguments(tk, &metadata->attributeList, &metadata->num_attributeList, &metadata->num_attributeCount);
		switch (r) {
			case TLArgumentsParseSuccess:
				break;
			case TLArgumentsParseMissingArgumentsError:
				// Covered by metadata
				break;
			case TLArgumentsParseUnexpectedTokenError:
				// Covered at beginning of this function
				break;
			case TLArgumentsParseOpenParenthesisError:
				logos_diagnoseToken(tk, token, CXDiagnostic_Error, "missing closing '(' to match this ')'");
				break;
		}
		if (r != TLArgumentsParseSuccess) {
			logos_directive_dispose(tk, metadata);
			return NULL;
		}

		for (CXToken token; logos_popToken(tk, &token);) {
			if (logos_checkKindAndStringOfToken(tk, token, CXToken_Punctuation, ";", NULL)) {
				break;
			}
			if (metadata->num_typeNameList == 0) {
				metadata->typeNameList = (CXToken *)calloc(1, sizeof(CXToken));
			} else {
				metadata->typeNameList = (CXToken *)realloc(metadata->typeNameList, (metadata->num_typeNameList + 1) * sizeof(CXToken));
			}
			metadata->typeNameList[metadata->num_typeNameList] = token;
			metadata->num_typeNameList++;
		}
	}
	return metadata;
}

static void logos_directive_describe(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		logos_diagnoseToken(tk, metadata->percentageToken, CXDiagnostic_Note,
			"found directive '%s'", logos_directive_name);

		printf("Attributes list:\n");
		for (unsigned int attributeIndex = 0; attributeIndex < metadata->num_attributeCount; attributeIndex++) {
			printf("- \033[33m");
			for (unsigned int index = 0; index < metadata->num_attributeList[attributeIndex]; index++) {
				CXToken token = metadata->attributeList[attributeIndex][index];
				CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
				const char * spelling_str = clang_getCString(spellingString);
				printf("%s", spelling_str);
				if (index < metadata->num_attributeList[attributeIndex] - 1) {
					printf(" ");
				}
				clang_disposeString(spellingString);
			}
			printf("\033[m\n");
		}

		printf("Type/Name:\n");
		printf("- \033[33m");
		for (unsigned int index = 0; index < metadata->num_typeNameList; index++) {
			CXToken token = metadata->typeNameList[index];
			CXString spellingString = clang_getTokenSpelling(tk->translationUnit, token);
			const char * spelling_str = clang_getCString(spellingString);
			printf("%s", spelling_str);
			if (index < metadata->num_typeNameList - 1) {
				printf(" ");
			}
			clang_disposeString(spellingString);
		}
		printf("\033[m\n");
	}
}

static void logos_directive_dispose(TLTokenizer tk, void * _metadata) {
	Metadata * metadata = (Metadata *)_metadata;
	if (metadata) {
		logos_disposeFunctionArguments(tk, metadata->attributeList, metadata->num_attributeList, metadata->num_attributeCount);
		if (metadata->typeNameList) {
			free(metadata->typeNameList);
		}
		free(metadata);
	}
}

void logos_directive_getInfo_property(TLDirective directive) {
	if (directive) {
		directive->name = (char *)logos_directive_name;
		directive->parse = logos_directive_parse;
		directive->describe = logos_directive_describe;
		directive->dispose = logos_directive_dispose;
	}
}
