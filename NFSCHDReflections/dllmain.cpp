#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

DWORD WINAPI Thing(LPVOID);

bool HDReflections, PseudoXbox360Reflections, DisableReflectionBlur;
static int ResolutionX, ResolutionY;
DWORD GameState;
HWND windowHandle;

DWORD RoadReflectionCodeCaveExit1 = 0x71AA2B;
DWORD RoadReflectionCodeCaveExit2 = 0x71AA7B;

void __declspec(naked) RoadReflectionCodeCave1()
{
	__asm {
		mov edx, dword ptr ds : [ResolutionX]
		push edx
		push eax
		call dword ptr ds : [ecx + 0x5C]
		jmp RoadReflectionCodeCaveExit1
	}
}

void __declspec(naked) RoadReflectionCodeCave2()
{
	__asm {
		mov ecx, dword ptr ds : [ResolutionX]
		push ecx
		push eax
		call dword ptr ds : [edx + 0x74]
		jmp RoadReflectionCodeCaveExit2
	}
}

void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSCHDReflections.ini");

	// Resolution
	ResolutionX = iniReader.ReadInteger("RESOLUTION", "ResolutionX", 1920);
	ResolutionY = iniReader.ReadInteger("RESOLUTION", "ResolutionY", 1080);

	// General
	HDReflections = iniReader.ReadInteger("GENERAL", "HDReflections", 1);
	PseudoXbox360Reflections = iniReader.ReadInteger("GENERAL", "PseudoXbox360Reflections", 1);
	DisableReflectionBlur = iniReader.ReadInteger("GENERAL", "DisableReflectionBlur", 0);

	if (HDReflections)
	{
		// Road Reflection X
		injector::MakeJMP(0x71AA26, RoadReflectionCodeCave1, true);
		injector::MakeJMP(0x71AA76, RoadReflectionCodeCave2, true);
		injector::WriteMemory<uint32_t>(0x71BE28, ResolutionX, true);
		// Road Reflection Y
		injector::MakeNOP(0x71A9FC, 2, true);
		injector::WriteMemory<uint32_t>(0x71A9F8, ResolutionY, true);
		injector::WriteMemory<uint32_t>(0x71BE2F, ResolutionY, true);
		// Vehicle Reflection
		injector::WriteMemory<uint32_t>(0x70DE39, ResolutionY, true);
	}

	if (PseudoXbox360Reflections)
	{
		injector::WriteMemory<uint8_t>(0x72ECE4, 0x02, true);
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
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.4 English nfsc.exe (6,88 MB (7.217.152 bytes)).", "NFSC HD Reflections", MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;
	
}
