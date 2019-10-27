//
//  main.m
//  uro_mh
//
//  Created by Ethan Arbuckle on 3/9/19.
//  Copyright © 2019 DataTheorem. All rights reserved.
//

#include <mach-o/dyld.h>
#include <mach-o/nlist.h> // struct nlist_64
#include <string.h> // strcmp

#include "dynamic.h"

#define ITER_ALWAYS 0
#define ITER_ONCE 1

static const struct mach_header * find_executable_header() {
	for (uint32_t i = 0; i < _dyld_image_count(); i++) {
		const struct mach_header * header = _dyld_get_image_header(i);
		if (header->filetype == MH_EXECUTE) {
			return header;
		}
	}
	return NULL;
}

static void iter_cmds_for_type(const struct mach_header * mh, uint32_t target, int only_once, void (^found)(struct load_command * lc)) {
	if (!mh) {
		return;
	}

    struct load_command * lcommand = (struct load_command *)((mach_vm_address_t)mh + sizeof(struct mach_header_64));
    for (uint32_t command = 0; command < mh->ncmds; command++) {
		lcommand = (struct load_command *)((mach_vm_address_t)lcommand + lcommand->cmdsize);
        if (lcommand->cmd == target) {
            found(lcommand);
            if (only_once) {
                return;
            }
        }
    }
}

void enumerate_symbols(const struct mach_header * mh, void (^callback)(const char * symbol, void * addr)) {
    if (!mh) {
        return;
    }

    __block uint64_t slide = 0;
    __block struct symtab_command * symcmd = NULL;
    __block struct nlist_64 * symtab = NULL;
    __block const char * strtab = NULL;

    iter_cmds_for_type(mh, LC_SYMTAB, ITER_ONCE, ^(struct load_command * lc) {
        symcmd = (struct symtab_command *)lc;
    });

    iter_cmds_for_type(mh, LC_SEGMENT_64, ITER_ALWAYS, ^(struct load_command * lc) {
        struct segment_command_64 * seg = (struct segment_command_64 *)lc;
        if (seg->fileoff == 0) {
            slide = (intptr_t)mh - seg->vmaddr;
        }

        if (strcmp(seg->segname, SEG_LINKEDIT) == 0) {
            symtab = (void *)seg->vmaddr + symcmd->symoff - seg->fileoff;
            strtab = (void *)seg->vmaddr + symcmd->stroff - seg->fileoff;
        }
    });

    if (!(slide && symtab && strtab)) {
        return;
    }

    symtab = (void *)symtab + slide;
    strtab = (void *)strtab + slide;

    for (uint32_t i = 0; i < symcmd->nsyms; i++) {
        struct nlist_64 * sym = &symtab[i];
		// Name
        uint32_t strx = sym->n_un.n_strx;
        const char * name = strx == 0 ? "-" : strtab + strx + 1; // why +1? skip '_' prefix
		// Address
        void * addr = (void *)sym->n_value + slide;

        if (!(sym->n_type & N_STAB) && (sym->n_type & N_TYPE) == N_SECT) {
            if (callback) {
                callback(name, addr);
            }
        }
    }
}

void enumerate_symbols_in_executable(void (^callback)(const char * symbol, void * addr)) {
	const struct mach_header * mh = find_executable_header();
	enumerate_symbols(mh, callback);
}
