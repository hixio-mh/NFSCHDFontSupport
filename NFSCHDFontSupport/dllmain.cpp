#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

bool HDFontSupport;

DWORD FontScaleCodeCave1Exit1 = 0x585D41;
DWORD FontScaleCodeCave1Exit2 = 0x585D28;
DWORD FontScaleCodeCave2Exit1 = 0x71CA12;
DWORD FontScaleCodeCave2Exit2 = 0x71CA0E;

void __declspec(naked) FontScaleCodeCave1()
{
	__asm {
	FontScaleCodeCave1Check:
		cmp dword ptr ds : [eax + 0x59], 0x544E4F46 // checks for "FONT"
		jne FontScaleCodeCave1None
		cmp dword ptr ds : [eax + 0x5D], 0x53464E5F // checks for "_NFS_"
		jne FontScaleCodeCave1None
		cmp dword ptr ds : [eax + 0x61], 0x444F425F // checks for "BODY" (FONT_NFS_BODY)
		je FontScaleCodeCave1Scale1
		cmp dword ptr ds : [eax + 0x61], 0x5449545F // checks for "TITLE" (FONT_NFS_TITLE)
		je FontScaleCodeCave1Scale2
		cmp dword ptr ds : [eax + 0x61], 0x564F4D5F // checks for "MOVIE" (FONT_NFS_MOVIE_LARGE)
		je FontScaleCodeCave1Scale3
		jmp FontScaleCodeCave1None

	FontScaleCodeCave1Scale1:
		mov edi, 0x0100 // 256: FONT_NFS_BODY
		mov dword ptr ds : [esp + 0x0C], esi
		mov dword ptr ds : [esp + 0x20], edi
		dec esi
		mov dword ptr ds : [esp + 0x08], esi
		fild dword ptr ds : [esp + 0x20]
		movzx esi, byte ptr ds : [edx + 0x02]
		fstp dword ptr ds : [esp + 0x20]
		mov eax, 0x0100 // 256: FONT_NFS_BODY
		jmp FontScaleCodeCave1Exit1

	FontScaleCodeCave1Scale2:
		mov edi, 0x0200 // 512: FONT_NFS_TITLE
		mov dword ptr ds : [esp + 0x0C] , esi
		mov dword ptr ds : [esp + 0x20] , edi
		dec esi
		mov dword ptr ds : [esp + 0x08] , esi
		fild dword ptr ds : [esp + 0x20]
		movzx esi, byte ptr ds : [edx + 0x02]
		fstp dword ptr ds : [esp + 0x20]
		mov eax, 0x0100 // 256: FONT_NFS_TITLE
		jmp FontScaleCodeCave1Exit1

	FontScaleCodeCave1Scale3:
		mov edi, 0x0200 // 512: FONT_NFS_MOVIE_LARGE
		mov dword ptr ds : [esp + 0x0C] , esi
		mov dword ptr ds : [esp + 0x20] , edi
		dec esi
		mov dword ptr ds : [esp + 0x08] , esi
		fild dword ptr ds : [esp + 0x20]
		movzx esi, byte ptr ds : [edx + 0x02]
		fstp dword ptr ds : [esp + 0x20]
		mov eax, 0x0200 // 512: FONT_NFS_MOVIE_LARGE
		jmp FontScaleCodeCave1Exit1

	FontScaleCodeCave1None:
		movsx edi, word ptr ds : [eax + 0x28]
		mov dword ptr ds : [esp + 0x0C], esi
		jmp FontScaleCodeCave1Exit2
	}
}

void __declspec(naked) FontScaleCodeCave2()
{
	__asm {
	FontScaleCodeCave2Check:
		cmp dword ptr ds : [ecx + 0x59], 0x544E4F46 // checks for "FONT"
		jne FontScaleCodeCave2None
		cmp dword ptr ds : [ecx + 0x5D], 0x53464E5F // checks for "_NFS_"
		jne FontScaleCodeCave2None
		cmp dword ptr ds : [ecx + 0x61], 0x444F425F // checks for "BODY" (FONT_NFS_BODY)
		je FontScaleCodeCave2Scale1
		cmp dword ptr ds : [ecx + 0x61], 0x5449545F // checks for "TITLE" (FONT_NFS_TITLE)
		je FontScaleCodeCave2Scale2
		cmp dword ptr ds : [ecx + 0x61], 0x564F4D5F // checks for "MOVIE" (FONT_NFS_MOVIE_LARGE)
		je FontScaleCodeCave2Scale3
		jmp FontScaleCodeCave2None

	FontScaleCodeCave2Scale1:
		mov edx, 0x0100 // 256: FONT_NFS_BODY
		mov dword ptr ds : [esp + 0x18] , edx
		mov eax, 0x0100 // 256: FONT_NFS_BODY
		jmp FontScaleCodeCave2Exit1
			
	FontScaleCodeCave2Scale2:
		mov edx, 0x0200 // 512: FONT_NFS_TITLE
		mov dword ptr ds : [esp + 0x18] , edx
		mov eax, 0x0100 // 256: FONT_NFS_TITLE
		jmp FontScaleCodeCave2Exit1

	FontScaleCodeCave2Scale3:
		mov edx, 0x0200 // 512: FONT_NFS_MOVIE_LARGE
		mov dword ptr ds : [esp + 0x18] , edx
		mov eax, 0x0200 // 512: FONT_NFS_MOVIE_LARGE
		jmp FontScaleCodeCave2Exit1

	FontScaleCodeCave2None:
		movsx edx, word ptr ds : [ecx + 0x28]
		mov dword ptr ds : [esp + 0x18] , edx
		jmp FontScaleCodeCave2Exit2
	}
}

void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSCHDFontSupport.ini");

	// General
	HDFontSupport = iniReader.ReadInteger("GENERAL", "HDFontSupport", 1);

	if (HDFontSupport)
	{
		injector::MakeJMP(0x585D20, FontScaleCodeCave1, true);
		injector::MakeJMP(0x71CA06, FontScaleCodeCave2, true);
	}
}
	

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x87E926) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
			Init();

		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.4 English nfsc.exe (6,88 MB (7.217.152 bytes)).", "NFSC HD Font Support", MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;
}