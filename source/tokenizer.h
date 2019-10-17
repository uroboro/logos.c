#ifndef LOGOS_TOKENIZER_H
#define LOGOS_TOKENIZER_H

#include <clang-c/Index.h>

#include "tlcommon.h"

#define LOGOS_EXPECTED_STRING_MAX_LENGTH   20
#define LOGOS_EXPECTED_NOTHING   0
#define LOGOS_EXPECTED_SOMETHING 1
#define LOGOS_EXPECTED_OPTIONAL  2


typedef struct TLTokenizerImpl * TLTokenizer;
typedef struct TLDirectiveImpl * TLDirective;
typedef struct TLMetadataImpl * TLMetadata;
typedef struct TLGroupImpl * TLGroup;

LOGOS_Struct(TLDirectiveImpl) {
	char * name;
	void * (*parse)(TLTokenizer tk, CXToken percentageToken);
	void (*describe)(TLTokenizer tk, void *);
	void (*dispose)(TLTokenizer tk, void *);
};

LOGOS_Struct(TLMetadataImpl) {
	unsigned int index; // Required first field in metadata structures
	// Rest of metadata
};

LOGOS_Struct(TLGroupImpl) {
	void * subclasses;
	unsigned int num_subclasses;

	void * hooks;
	unsigned int num_hooks;

	void * hookfs;
	unsigned int num_hookfs;
};


struct TLTokenizerImpl {
	char filePath[256];
	CXTranslationUnit translationUnit;
	CXToken * tokens;
	unsigned int num_tokens;
	unsigned int offset;

	TLDirectiveImpl * directives;
	unsigned int num_directives;

	// Found metadata in module
	void * * metadata;
	unsigned int num_metadata;
#if 0
	// Found groups in module
	TLGroup * groups;
	unsigned int num_groups;

	// Found constructors in module
	void * * constructors;
	unsigned int num_constructors;

	// Found destructors in module
	void * * destructors;
	unsigned int num_destructors;

	// Found initializers in module
	void * * initializers;
	unsigned int num_initializers;
#endif
};


TLTokenizer logos_createTokenizer(const char * filepath);

void logos_disposeTokenizer(TLTokenizer tk);

void logos_dumpToken(CXTranslationUnit tu, unsigned int index, CXToken token, int useColors);

void logos_dumpTokens(TLTokenizer tk, int useColors);

int logos_peekToken(TLTokenizer tk, CXToken * token);

int logos_popToken(TLTokenizer tk, CXToken * token);

int logos_lastToken(TLTokenizer tk, CXToken * token);

void logos_rewindTokens(TLTokenizer tk);

void logos_diagnoseExpectedToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * expectation);

void logos_diagnoseExpectedAfterToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * expectation);

void logos_diagnoseToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * format, ...);

void logos_diagnoseAfterToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * format, ...);

int logos_checkKindAndStringOfToken(TLTokenizer tk, CXToken token, CXTokenKind target_kind, ... /* NULL-terminated `char *` list */);

LOGOS_Enum(TLArgumentsParseEnum) {
	TLArgumentsParseSuccess,
	TLArgumentsParseMissingArgumentsError,
	TLArgumentsParseUnexpectedTokenError,
	TLArgumentsParseOpenParenthesisError,
};

LOGOS_EXTERN CXToken logos_openParenthesisToken;
TLArgumentsParseEnum logos_parseFunctionArguments(TLTokenizer tk, CXToken * * * _argumentList, unsigned int * * _num_argumentList, unsigned int * _num_argumentCount);

void logos_disposeFunctionArguments(TLTokenizer tk, CXToken * * argumentList, unsigned int * num_argumentList, unsigned int num_argumentCount);


void logos_analyseTokens(TLTokenizer tk);

#endif /* LOGOS_TOKENIZER_H */
