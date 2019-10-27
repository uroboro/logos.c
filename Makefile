export TARGET = native:clang:latest
export ARCHS = x86_64

include $(THEOS)/makefiles/common.mk

# CLANG_LIBS = clang clangARCMigrate clangAST clangASTMatchers clangAnalysis clangApplyReplacements clangBasic clangCodeGen clangDriver clangDynamicASTMatchers clangEdit clangFormat clangFrontend clangFrontendTool clangIndex clangLex clangParse clangQuery clangRename clangRewrite clangRewriteFrontend clangSema clangSerialization clangStaticAnalyzerCheckers clangStaticAnalyzerCore clangStaticAnalyzerFrontend clangTidy clangTidyGoogleModule clangTidyLLVMModule clangTidyMiscModule clangTidyReadability clangTidyUtils clangTooling clangToolingCore
# CLANG_LIBS += z ncurses
# LLVM_LIBS = $(shell llvm-config-3.7 --libs)

TOOL_NAME = logos

logos_FILES = $(call findfiles, source) $(call findfiles, directives)
logos_LIBRARIES = clang
# logos_CFLAGS = -Weverything

logos_CFLAGS = $(shell llvm-config-3.7 --cflags)
logos_CCFLAGS = $(shell llvm-config-3.7 --cxxflags)
logos_LDFLAGS = $(shell llvm-config-3.7 --ldflags)

include $(THEOS_MAKE_PATH)/tool.mk

test-logos: logos
	# @bat --paging=never $(ARGS)
	@$(THEOS_OBJ_DIR)/logos $(ARGS)

print:
	@echo $(THEOS_OBJ_DIR)/logos

# define TEST_RULE
# test-$(1): $(1)
# 	@$(THEOS_OBJ_DIR)/$(1) $(ARGS)
# endef
#
# $(foreach _tool, $(TOOL_NAME), $(call TEST_RULE, $(_tool)))
