#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

bool HDFontSupport;

DWORD FontScaleCodeCave1Exit1 = 0x585D41;
DWORD FontScaleCodeCave1Part2Exit = 0x585D28;
DWORD FontScaleCodeCave2Exit1 = 0x71CA12;
DWORD FontScaleCodeCave2Part2Exit = 0x71CA0E;

void __declspec(naked) FontScaleCodeCave1()
{
	__asm {
		cmp dword ptr ds : [eax + 0x59], 0x544E4F46 // checks for "FONT"
		jne FontScaleCodeCave1Part2
		cmp dword ptr ds : [eax + 0x5D], 0x53464E5F // checks for "_NFS_"
		jne FontScaleCodeCave1Part2
		cmp dword ptr ds : [eax + 0x61], 0x444F425F // checks for "BODY"
		je FontScaleCodeCave1Scale1
		cmp dword ptr ds : [eax + 0x61], 0x5449545F // checks for "TITLE"
		je FontScaleCodeCave1Scale2
		jmp FontScaleCodeCave1Part2

	FontScaleCodeCave1Scale1:
		mov edi, 0x0100 // 256 FONT_NFS_BODY scale
		mov dword ptr ds : [esp + 0x0C], esi
		mov dword ptr ds : [esp + 0x20], edi
		dec esi
		mov dword ptr ds : [esp + 0x08], esi
		fild dword ptr ds : [esp + 0x20]
		movzx esi, byte ptr ds : [edx + 0x02]
		fstp dword ptr ds : [esp + 0x20]
		mov eax, 0x0100 // 256 FONT_NFS_BODY scale
		jmp FontScaleCodeCave1Exit1

	FontScaleCodeCave1Scale2 :
		mov edi, 0x0200 // 512 FONT_NFS_TITLE scale
		mov dword ptr ds : [esp + 0x0C] , esi
		mov dword ptr ds : [esp + 0x20] , edi
		dec esi
		mov dword ptr ds : [esp + 0x08] , esi
		fild dword ptr ds : [esp + 0x20]
		movzx esi, byte ptr ds : [edx + 0x02]
		fstp dword ptr ds : [esp + 0x20]
		mov eax, 0x0100 // 256 FONT_NFS_TITLE scale
		jmp FontScaleCodeCave1Exit1

	FontScaleCodeCave1Part2:
		movsx edi, word ptr ds : [eax + 0x28]
		mov dword ptr ds : [esp + 0x0C], esi
		jmp FontScaleCodeCave1Part2Exit
	}
}

void __declspec(naked) FontScaleCodeCave2()
{
	__asm {
		cmp dword ptr ds : [ecx + 0x59] , 0x544E4F46 // checks for "FONT"
		jne FontScaleCodeCave2Part2
		cmp dword ptr ds : [ecx + 0x5D] , 0x53464E5F // checks for "_NFS_"
		jne FontScaleCodeCave2Part2
		cmp dword ptr ds : [ecx + 0x61] , 0x444F425F // checks for "BODY"
		je FontScaleCodeCave2Scale1
		cmp dword ptr ds : [ecx + 0x61] , 0x5449545F // checks for "TITLE"
		je FontScaleCodeCave2Scale2
		jmp FontScaleCodeCave2Part2

	FontScaleCodeCave2Scale1 :
		mov edx, 0x0100 // 256 FONT_NFS_BODY scale
		mov dword ptr ds : [esp + 0x18] , edx
		mov eax, 0x0100 // 256 FONT_NFS_BODY scale
		jmp FontScaleCodeCave2Exit1
			
	FontScaleCodeCave2Scale2 :
		mov edx, 0x0200 // 512 FONT_NFS_TITLE scale
		mov dword ptr ds : [esp + 0x18] , edx
		mov eax, 0x0100 // 256 FONT_NFS_TITLE scale
		jmp FontScaleCodeCave2Exit1

	FontScaleCodeCave2Part2 :
		movsx edx, word ptr ds : [ecx + 0x28]
		mov dword ptr ds : [esp + 0x18] , edx
		jmp FontScaleCodeCave2Part2Exit
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