#ifndef LOGOS_DIAGNOSE_H
#define LOGOS_DIAGNOSE_H

#include <stdarg.h>

#include "tokenizer.h"

void logos_diagnosePrintFileLocation(TLTokenizer tk, CXToken token);

void logos_diagnosePrintSeverity(TLTokenizer tk, enum CXDiagnosticSeverity severity);

void logos_diagnosePrintWithSeverity_v(TLTokenizer tk, enum CXDiagnosticSeverity severity, const char * format, va_list ap);

void logos_diagnosePrintWithSeverity(TLTokenizer tk, enum CXDiagnosticSeverity severity, const char * format, ...);

void logos_diagnosePrintFileLine(TLTokenizer tk, unsigned line);

void logos_diagnosePrintAtLocation(TLTokenizer tk, unsigned line, unsigned column, const char * format, ...);

void logos_diagnoseExpectedTokenAtLocation(TLTokenizer tk, CXToken token, CXSourceLocation location, enum CXDiagnosticSeverity severity, const char * expectation);

void logos_diagnoseExpectedToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * expectation);

void logos_diagnoseExpectedAfterToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * expectation);

void logos_diagnoseToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * format, ...);

void logos_diagnoseAfterToken(TLTokenizer tk, CXToken token, enum CXDiagnosticSeverity severity, const char * format, ...);

#endif /* LOGOS_DIAGNOSE_H */
