#ifndef PAGE_H
#define PAGE_H

#include <Windows.h>

#define RANDOM_NUMB(min, max) (rand() % (max + 1 - min) + min)
#define ALIGN_PAGE(n) ((n + 0x1000) & ~(0x1000))

#define FACTOR 2048

typedef struct _PAGE_SHELLCODE_CONTEXT {
	UINT8 u8Key;
	DWORD dwLocation;
	SIZE_T uSize;
	LPVOID lpPage;
} PAGE_SHELLCODE_CONTEXT, *PPAGE_SHELLCODE_CONTEXT;

#ifdef _DEBUG
VOID 
dump_ctx_struct(
	PPAGE_SHELLCODE_CONTEXT pCtx
);
#endif

PPAGE_SHELLCODE_CONTEXT 
allocate_large_page(
	HANDLE hTarget,
	DWORD szPageSize
);

VOID 
place_shellcode_rand(
	PPAGE_SHELLCODE_CONTEXT ctx,
	PBYTE pbBuffer,
	SIZE_T szBufferLen
);

VOID 
execute_shellcode(
	PPAGE_SHELLCODE_CONTEXT psCtx
);

VOID 
destroy_context(
	PPAGE_SHELLCODE_CONTEXT ctx
);

#endif