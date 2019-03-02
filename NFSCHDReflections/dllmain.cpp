#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

DWORD WINAPI Thing(LPVOID);

bool HDReflections, PseudoXbox360Reflections;
static int ResolutionX, ResolutionY;
static float ReflectionBlurStrength;
static float DownScale4x4Strength = 2.0f;
DWORD GameState;
HWND windowHandle;

DWORD RoadReflectionCodeCaveExit1 = 0x71AA2B;
DWORD RoadReflectionCodeCaveExit2 = 0x71AA7B;
DWORD ReflectionBlurStrengthCodeCaveExit = 0x73C982;
DWORD DownScale4x4ReturnAddress;
DWORD DownScale4x4ReturnAddressCodeCaveExit = 0x748886;
DWORD DownScale4x4StrengthCodeCaveExit1 = 0x73CC2F;
DWORD DownScale4x4StrengthCodeCaveExit2 = 0x73CC43;

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

void __declspec(naked) ReflectionBlurStrengthCodeCave()
{
	__asm {
		fdivr dword ptr ds : [ReflectionBlurStrength]
		jmp ReflectionBlurStrengthCodeCaveExit
	}
}

void __declspec(naked) DownScale4x4ReturnAddressCodeCave()
{
	__asm {
		// gets return address
		push eax
		mov eax, dword ptr ds : [esp + 0x4]
		mov dword ptr ds : [DownScale4x4ReturnAddress], eax
		pop eax
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF0
		jmp DownScale4x4ReturnAddressCodeCaveExit
	}
}


void __declspec(naked) DownScale4x4StrengthCodeCave1()
{
	__asm {
		// horizontal road reflection
		cmp dword ptr ds : [DownScale4x4ReturnAddress], 0x72E22E
		jne DownScale4x4StrengthCodeCave1Part2
		fdivr dword ptr ds : [ReflectionBlurStrength]
		jmp DownScale4x4StrengthCodeCaveExit1
		
	DownScale4x4StrengthCodeCave1Part2 :
		// restores horizontal depth of field
		fdivr dword ptr ds : [DownScale4x4Strength]
		jmp DownScale4x4StrengthCodeCaveExit1

		
	}
}

void __declspec(naked) DownScale4x4StrengthCodeCave2()
{
	__asm {
		// vertical road reflection
		cmp dword ptr ds : [DownScale4x4ReturnAddress], 0x72E22E
		jne DownScale4x4StrengthCodeCave2Part2
		fdivr dword ptr ds : [ReflectionBlurStrength]
		jmp DownScale4x4StrengthCodeCaveExit2

	DownScale4x4StrengthCodeCave2Part2 :
		// restores vertical depth of field
		fdivr dword ptr ds : [DownScale4x4Strength]
		jmp DownScale4x4StrengthCodeCaveExit2
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
	ReflectionBlurStrength = iniReader.ReadFloat("GENERAL", "ReflectionBlurStrength", 0.0f);

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

	// ReflectionBlurStength
	{
		// Jumps
		injector::MakeJMP(0x73C97C, ReflectionBlurStrengthCodeCave);
		injector::MakeJMP(0x748880, DownScale4x4ReturnAddressCodeCave);
		injector::MakeJMP(0x73CC29, DownScale4x4StrengthCodeCave1);
		injector::MakeJMP(0x73CC3D, DownScale4x4StrengthCodeCave2);
		// Allows blur to render at full resolution
		injector::MakeNOP(0x713108, 2);
		injector::MakeNOP(0x713141, 2);
		injector::MakeNOP(0x73C359, 2);
		injector::MakeNOP(0x73C371, 2);
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
