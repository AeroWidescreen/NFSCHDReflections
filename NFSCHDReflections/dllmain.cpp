#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

bool HDReflections, OldGPUCompatibility, AlignmentFix, AspectRatioFix, RestoreSkybox, XB360Reflections, EnableMirror, SynchronizeVisualTreatment, RealFrontEndReflections, CubemapFix, TrafficSignFix, PauseBlur, RealisticChrome;
int ImproveReflectionLOD, ReplaceVisualTreatment;

int RoadResX = 320;
int RoadResY = 240;
int VehicleRes = 256;
int MirrorResX = 512;
int MirrorResY = 256;
int PIPRes = 256;

float RoadScale, VehicleScale, MirrorScale, PIPScale, VehicleReflectionBrightness;
float ReflectionBlurStrength;
float DOFStrength = 1.0f;
float BloomBlurStrength = 1.0f;
float ReflectionAspectRatioOriginal = 0.585f;
float ReflectionAspectRatioCorrectedAlignmentFix = 0.0f;
float ReflectionAspectRatioCorrected = -0.6f;
float TrafficSignDistance = 45000.0f;
float PauseBlurValue1 = 1.0f;
float PauseBlurValue2 = 10.0f;
float VehicleReflSkyboxRenderDistance = 0.25f;
float VehicleReflBrightnessAddition = 0.0f;
float VehicleReflBrightnessSubtraction = 1.6f;
float VehicleReflBrightnessAddition2 = 0.0f;
float VehicleReflBrightnessSubtraction2 = 0.95f;
float FEVehicleReflBrightnessAddition = 2.5f;
float FEVehicleReflBrightnessSubtraction = 1.0f;
float FEReflBrightness = 0.15f;
float VehicleReflSkyboxBrightnessSubtraction = 1.75f;
__int64 VehicleReflBrightness;
__int64 VehicleReflBrightness2;
__int64 FEVehicleReflBrightness;
__int64 VehicleReflSkyboxBrightness;

DWORD RoadReflectionCodeCave1Exit = 0x71AA2B;
DWORD RoadReflectionRes1;
DWORD RoadReflectionCodeCave2Exit = 0x71AA7B;
DWORD RoadReflectionRes2;
DWORD RoadReflectionCodeCave3Exit = 0x71A9FE;
DWORD RoadReflectionRes3;
DWORD TrafficSignFixCodeCaveExit = 0x71E085;
DWORD RealisticChromeCodeCaveExit = 0x71E08F;
DWORD ImproveReflectionLODCodeCave1Exit = 0x79FB65;
DWORD ImproveReflectionLODCodeCave2Exit = 0x79AEE1;
DWORD ImproveReflectionLODCodeCave3Exit = 0x79AE9A;
DWORD VehicleReflShaderCodeCaveExit = 0x729461;
DWORD VehicleReflShaderSettingCodeCaveExit = 0x7236D5;
DWORD AlignmentFixCodeCaveExit = 0x71B8A6;
DWORD VehicleReflAspectRatioCodeCaveFrontEndJump = 0x73E1C1;
DWORD VehicleReflAspectRatioCodeCaveExit = 0x73E1BA;
DWORD VehicleReflSkyboxCodeCaveExit = 0x7AFCAA;
DWORD VehicleReflSkyboxCodeCavePart2Exit = 0x7AFCD5;
DWORD VehicleReflSkyboxCodeCavePart2Jump = 0x7AFCAA;
DWORD VehicleReflBrightnessCodeCaveExit = 0x748A9D;
DWORD VehicleReflBrightnessCodeCave2Exit = 0x7497E4;
DWORD VehicleReflSkyboxBrightnessCodeCaveExit = 0x7498B0;
DWORD FEReflBrightnessCodeCaveExit = 0x71E24F;
DWORD FEReflBrightnessCodeCavePart2Exit = 0x71E1A8;
DWORD VehicleReflRenderDistanceCodeCaveExit = 0x721DEB;
DWORD sub_713B30 = 0x713B30;
DWORD VehicleReflFlareSizeCodeCaveExit = 0x74D9DC;
DWORD VisualTreatmentCodeCaveExit1 = 0x71D706;
DWORD VisualTreatmentCodeCaveExit2 = 0x71D700;

void __declspec(naked) RoadReflectionCodeCave1()
{
	__asm {
	// for widescreen fix compatibility
		fild dword ptr ds : [RoadResX]
		fmul dword ptr ds : [RoadScale]
		fistp dword ptr ds : [RoadReflectionRes1]
		mov edx, dword ptr ds : [RoadReflectionRes1]
		push edx
		push eax
		call dword ptr ds : [ecx + 0x5C]
		jmp RoadReflectionCodeCave1Exit
	}
}

void __declspec(naked) RoadReflectionCodeCave2()
{
	__asm {
	// for widescreen fix compatibility
		fild dword ptr ds : [RoadResX]
		fmul dword ptr ds : [RoadScale]
		fistp dword ptr ds : [RoadReflectionRes2]
		mov ecx, dword ptr ds : [RoadReflectionRes2]
		push ecx
		push eax
		call dword ptr ds : [edx + 0x74]
		jmp RoadReflectionCodeCave2Exit
	}
}

void __declspec(naked) RoadReflectionCodeCave3()
{
	__asm {
	// for widescreen fix compatibility
		mov eax, dword ptr ds : [0xAB0ABC]
		fild dword ptr ds : [RoadResY]
		fmul dword ptr ds : [RoadScale]
		fistp dword ptr ds : [RoadReflectionRes3]
		mov edx, dword ptr ds : [RoadReflectionRes3]
		jmp RoadReflectionCodeCave3Exit
	}
}

void __declspec(naked) DownScale4x4StrengthCodeCave()
{
	__asm {
		cmp dword ptr ds : [ebp + 0x04], 0x72E22E // Road Reflection Return Address
		je DownScale4x4StrengthCodeCaveRoad
		cmp dword ptr ds : [ebp + 0x04], 0x726B17 // Bloom Return Address
		je DownScale4x4StrengthCodeCaveBloom
		
	// restores horizontal depth of field
		fdivr dword ptr ds : [DOFStrength]
		ret

	DownScale4x4StrengthCodeCaveBloom:
	// restores horizontal frontend blur
		fdivr dword ptr ds : [BloomBlurStrength]
		ret
		
	DownScale4x4StrengthCodeCaveRoad:
	// horizontal road reflection
		fdivr dword ptr ds : [ReflectionBlurStrength]
		ret
	}
}

void __declspec(naked) TrafficSignFixCodeCave()
{
	__asm {
		mov ecx, dword ptr ds : [0xA6B910]
		mov dword ptr ds : [ecx + 0x74], 0x41000000 // Traffic sign materiel brightness
		fld dword ptr ds : [esi + 0x6C]
		fmul dword ptr ds : [esi + 0x64]
		jmp TrafficSignFixCodeCaveExit
	}
}

void __declspec(naked) RealisticChromeCodeCave()
{
	__asm {
		cmp byte ptr ds : [0xA63E60], 0x03 // checks if Shader Detail = High
		jne NoRealisticChromeCodeCave
		cmp byte ptr ds : [0xA65340], 0x00 // checks if Reflection Update Rate is greater than 0
		jle NoRealisticChromeCodeCave
		cmp byte ptr ds : [0xA63E77], 0x00 // checks if vehicle reflections are enabled
		jle NoRealisticChromeCodeCave
		mov ecx, dword ptr ds : [0xA6B910]
		mov dword ptr ds : [ecx - 0xB90], 0x40C00000 // Chrome Materiel Intensity: 6.0f
		mov dword ptr ds : [ecx - 0xBF0], 0x3E000000 // Chrome Materiel Brightness: 0.125f
		mov dword ptr ds : [ecx - 0xC00], 0x3E800000 // Chrome Materiel Reflect Brightness: 0.25f
		jmp RealisticChromeCodeCavePart2

	NoRealisticChromeCodeCave:
		mov ecx, dword ptr ds : [0xA6B910]
		mov dword ptr ds : [ecx - 0xB90], 0x3F800000 // Chrome Materiel Intensity
		mov dword ptr ds : [ecx - 0xBF0], 0x3F800000 // Chrome Materiel Brightness
		mov dword ptr ds : [ecx - 0xC00], 0x3F800000 // Chrome Materiel Reflect Brightness

	RealisticChromeCodeCavePart2:
		fld dword ptr ds : [esi + 0x70]
		fmul dword ptr ds : [esi + 0x64]
		jmp RealisticChromeCodeCaveExit
	}
}

void __declspec(naked) ImproveReflectionLODCodeCave1()
{
	__asm {

		cmp dword ptr ds : [0xA99BBC], 0x06
		jne ImproveReflectionLODCodeCave1FrontEnd

	// ImproveReflectionLODCodeCave1InGame
		cmp byte ptr ds : [ImproveReflectionLOD], 0x00
		jg ImproveReflectionLODCodeCave1On
		jmp ImproveReflectionLODCodeCave1Off
		
	ImproveReflectionLODCodeCave1FrontEnd:
		cmp byte ptr ds : [RealFrontEndReflections], 0x00
		jg ImproveReflectionLODCodeCave1On
		jmp ImproveReflectionLODCodeCave1Off

	ImproveReflectionLODCodeCave1Off:
		test ch, 0x30
		mov dword ptr ds : [esp + 0x14], 0x00000000
		jmp ImproveReflectionLODCodeCave1Exit

	ImproveReflectionLODCodeCave1On:
		test ch, 0x10
		mov dword ptr ds : [esp + 0x14], 0x00000000
		jmp ImproveReflectionLODCodeCave1Exit
	}
}

void __declspec(naked) ImproveReflectionLODCodeCave2()
{
	__asm {

		cmp dword ptr ds : [0xA99BBC], 0x06
		jne ImproveReflectionLODCodeCave2FrontEnd

	// ImproveReflectionLODCodeCave2InGame
		cmp byte ptr ds : [ImproveReflectionLOD], 0x00
		jg ImproveReflectionLODCodeCave2On
		jmp ImproveReflectionLODCodeCave2Off

	ImproveReflectionLODCodeCave2FrontEnd:
		cmp byte ptr ds : [RealFrontEndReflections], 0x00
		jg ImproveReflectionLODCodeCave2On
		jmp ImproveReflectionLODCodeCave2Off

	ImproveReflectionLODCodeCave2Off:
		or edx, 0x08002180
		jmp ImproveReflectionLODCodeCave2Exit

	ImproveReflectionLODCodeCave2On:
		or edx, 0x00008002
		jmp ImproveReflectionLODCodeCave2Exit
	}
}

void __declspec(naked) ImproveReflectionLODCodeCave3()
{
	__asm {
		cmp dword ptr ds : [ebx + 0x08], 0x03
		je ImproveReflectionLODCodeCave3Part2
		mov edi, dword ptr ds : [ebx + 0x08]
		lea ecx, dword ptr ds : [edx + edx * 0x04]
		jmp ImproveReflectionLODCodeCave3Exit

	ImproveReflectionLODCodeCave3Part2:
	// for mirror LOD
		mov edi, 0x01
		lea ecx, dword ptr ds : [edx + edx * 0x04]
		jmp ImproveReflectionLODCodeCave3Exit
	}
}

void __declspec(naked) VehicleReflShaderCodeCave()
{
	__asm {
		mov ecx, dword ptr ds : [edi + 0x04]
	// excludes geometry
		cmp ecx, 0x01
		je VehicleReflShaderCodeCavePart2
		jmp VehicleReflShaderCodeCaveExit

	VehicleReflShaderCodeCavePart2 :
		push edi
		mov edi, [esp + 0x0C]
		mov edi, [edi + 0x08]
	// checks for vehicle reflection
		cmp edi, 0x16
		jnl VehicleReflShaderCodeCavePart3
		pop edi
		jmp VehicleReflShaderCodeCaveExit

	VehicleReflShaderCodeCavePart3 :
		pop edi
		movzx ecx, ax
		mov edi, dword ptr ds : [ecx * 0x4 + 0xB1F66C]
		jmp VehicleReflShaderCodeCaveExit
	}
}

void __declspec(naked) VehicleReflShaderSettingCodeCave()
{
	_asm
	{
		mov eax, dword ptr ds : [0xB42FC8]
		mov dword ptr ds : [eax + 0x1774], 0x02 // lowers shader detail of traffic signs
		mov eax, dword ptr ds : [ebx + 0x1774]
		jmp VehicleReflShaderSettingCodeCaveExit
	}
}


void __declspec(naked) AlignmentFixCodeCave()
{
	__asm {
		cmp eax, 0x16
		jl AlignmentFixCodeCaveConditional
		mov dword ptr ds : [esp + 0x24], 0x3F800000 

	AlignmentFixCodeCaveConditional:
		movzx ecx, word ptr ds : [edi + 0xE4]
		jmp AlignmentFixCodeCaveExit
	}
}

void __declspec(naked) VehicleReflAspectRatioCodeCave()
{
	__asm {
		mov eax, [0xA99BBC]
	// checks if AspectRatioFix is enabled
		cmp byte ptr ds : [AspectRatioFix], 0x01 
		jge VehicleReflAspectRatioCodeCaveCorrected

	// checks if frontend or in-game
		cmp eax, 0x06
		je VehicleReflAspectRatioCodeCaveOriginal
	// jumps to 0x73E1C1 if frontend
		jmp VehicleReflAspectRatioCodeCaveFrontEndJump
	
	VehicleReflAspectRatioCodeCaveOriginal:
	// if frontend and AspectRatioFix is disabled
		mov eax, dword ptr ds : [ReflectionAspectRatioOriginal]
		jmp VehicleReflAspectRatioCodeCavePart2
			
	VehicleReflAspectRatioCodeCaveCorrected:
	// checks if AlignmentFix is enabled
		cmp byte ptr ds : [AlignmentFix], 0x01
		jge VehicleReflAspectRatioCodeCaveCorrectedAlignmentFix
	// if AlignmentFix is disabled
		mov eax, dword ptr ds : [ReflectionAspectRatioCorrected]
		jmp VehicleReflAspectRatioCodeCavePart2
	VehicleReflAspectRatioCodeCaveCorrectedAlignmentFix:
	// if AlignmentFix is enabled
		mov eax, dword ptr ds : [ReflectionAspectRatioCorrectedAlignmentFix]

	VehicleReflAspectRatioCodeCavePart2:
		push eax
		mov ecx, esi
		push 0x37
		jmp VehicleReflAspectRatioCodeCaveExit
	}
}

void __declspec(naked) VehicleReflSkyboxCodeCave()
{
	__asm {
		cmp eax, 0x03
		jg VehicleReflSkyboxCodeCavePart2
		jmp VehicleReflSkyboxCodeCaveExit

	VehicleReflSkyboxCodeCavePart2 :
		mov eax, dword ptr ds : [0xA7335C]
		test eax, eax
		je VehicleReflSkyboxCodeCavePart2Conditional
		test ecx, ecx
		fld dword ptr ds : [VehicleReflSkyboxRenderDistance]
		fmul st(0), st(1)
		fmul dword ptr ds: [esi]
		fstp dword ptr ds : [esi]
		fld dword ptr ds : [VehicleReflSkyboxRenderDistance]
		fmul st(0), st(1)
		fmul dword ptr ds : [esi + 0x14]
		fstp dword ptr ds : [esi + 0x14]
		fld dword ptr ds : [VehicleReflSkyboxRenderDistance]
		jmp VehicleReflSkyboxCodeCavePart2Exit

	VehicleReflSkyboxCodeCavePart2Conditional :
		jmp VehicleReflSkyboxCodeCavePart2Jump
	}
}

void __declspec(naked) VehicleReflBrightnessCodeCave()
{
	__asm {
		push edx
		mov edx, dword ptr ds : [0xAB08F4]
		mov edx, dword ptr ds : [edx + 0x04]
		cmp edx, 0x16
		jl VehicleReflBrightnessCodeCavePart2
		push ebx
		lea ebx, dword ptr ds : [VehicleReflBrightness]
		fld dword ptr ds : [eax]
		fadd dword ptr ds : [VehicleReflBrightnessAddition]
		fstp dword ptr ds : [ebx]
		fld dword ptr ds : [eax + 0x4]
		fsub dword ptr ds : [VehicleReflBrightnessSubtraction]
		fstp dword ptr ds : [ebx + 0x04]
		mov eax, ebx
		pop ebx
		pop edx
		push eax
		push 0x8B
		jmp VehicleReflBrightnessCodeCaveExit

	VehicleReflBrightnessCodeCavePart2:
		pop edx
		push eax
		push 0x8B
		jmp VehicleReflBrightnessCodeCaveExit
	}
}

void __declspec(naked) VehicleReflBrightnessCodeCave2()
{
	__asm {
		push edx
		mov edx, dword ptr ds : [0xAB08F4]
		mov edx, dword ptr ds : [edx + 0x04]
		cmp edx, 0x16
		jl VehicleReflBrightnessCodeCave2Part2
		push ebx
		lea ebx, dword ptr ds : [FEVehicleReflBrightness]
		cmp dword ptr ds : [0xA99BBC] , 0x06
		je InGameVehicleReflBrightnessCodeCave
		cmp byte ptr ds : [XB360Reflections], 0x00
		jle VehicleReflBrightnessCodeCave2Part2

	// FEVehicleReflBrightnessCodeCave
		fld dword ptr ds : [eax]
		fadd dword ptr ds : [FEVehicleReflBrightnessAddition]
		fstp dword ptr ds : [ebx]
		fld dword ptr ds : [eax + 0x04]
		fsub dword ptr ds : [FEVehicleReflBrightnessSubtraction]
		fstp dword ptr ds : [ebx + 0x04]
		mov eax, ebx
		pop ebx
		pop edx
		push eax
		push 0x8B
		jmp VehicleReflBrightnessCodeCave2Exit

	InGameVehicleReflBrightnessCodeCave:
		lea ebx, dword ptr ds : [VehicleReflBrightness2]
		fld dword ptr ds : [eax]
		fadd dword ptr ds : [VehicleReflBrightnessAddition2]
		fstp dword ptr ds : [ebx]
		fld dword ptr ds : [eax + 0x04]
		fsub dword ptr ds : [VehicleReflBrightnessSubtraction2]
		fstp dword ptr ds : [ebx + 0x04]
		mov eax, ebx
		pop ebx
		pop edx
		push eax
		push 0x8B
		jmp VehicleReflBrightnessCodeCave2Exit

	VehicleReflBrightnessCodeCave2Part2:
		pop edx
		push eax
		push 0x8B
		jmp VehicleReflBrightnessCodeCave2Exit
	}
}

void __declspec(naked) VehicleReflSkyboxBrightnessCodeCave()
{
	__asm {
		push edx
		cmp dword ptr ds : [0xA99BBC], 0x06
		je VehicleReflSkyboxBrightnessCodeCavePart2
		mov edx, dword ptr ds : [0xAB08F4]
		mov edx, dword ptr ds : [edx + 0x04]
		cmp edx, 0x16
		jl VehicleReflSkyboxBrightnessCodeCavePart2
		push ebx
		lea ebx, dword ptr ds : [VehicleReflSkyboxBrightness]
		fld dword ptr ds : [eax]
		fstp dword ptr ds : [ebx]
		fld dword ptr ds : [eax + 0x04]
		fsub dword ptr ds : [VehicleReflSkyboxBrightnessSubtraction]
		fstp dword ptr ds : [ebx + 0x04]
		mov eax, ebx
		pop ebx
		pop edx
		push eax
		push 0x8B
		jmp VehicleReflSkyboxBrightnessCodeCaveExit

		VehicleReflSkyboxBrightnessCodeCavePart2 :
		pop edx
		push eax
		push 0x8B
		jmp VehicleReflSkyboxBrightnessCodeCaveExit
	}
}

void __declspec(naked) FEReflBrightnessCodeCave()
{
	__asm {
		fmul dword ptr ds : [esi + 0xC4]
		cmp dword ptr ds : [0xA99BBC], 0x06
		je FEReflBrightnessCodeCavePart2
		cmp byte ptr ds : [XB360Reflections], 0x00
		jg FEReflBrightnessCodeCavePart2
		fmul dword ptr ds : [FEReflBrightness]
		fmul dword ptr ds : [VehicleReflectionBrightness]
		fstp dword ptr ds : [esp + 0x30]
		fld dword ptr ds : [esi + 0xCC]
		fmul dword ptr ds : [esi + 0xC4]
		fmul dword ptr ds : [FEReflBrightness]
		fmul dword ptr ds : [VehicleReflectionBrightness]
		fstp dword ptr ds : [esp + 0x34]
		fld dword ptr ds : [esi + 0xD0]
		fmul dword ptr ds : [esi + 0xC4]
		fmul dword ptr ds : [FEReflBrightness]
		fmul dword ptr ds : [VehicleReflectionBrightness]
		fstp dword ptr ds : [esp + 0x38]
		fld dword ptr ds : [esi + 0xD8]
		fmul dword ptr ds : [esi + 0xD4]
		mov eax, dword ptr ds : [esi + 0x8C]
		mov edx, dword ptr ds : [esi + 0x54]
		mov ecx, dword ptr ds : [esi + 0xB8]
		fmul dword ptr ds : [FEReflBrightness]
		fmul dword ptr ds : [VehicleReflectionBrightness]
		mov dword ptr ds : [esp + 0x54], eax
		mov dword ptr ds : [esp + 0x7C], 0x00
		mov dword ptr ds : [esp + 0x50], edx
		fsub dword ptr ds : [esp + 0x30]
		mov dword ptr ds : [esp + 0x58], ecx
		mov dword ptr ds : [esp + 0x5C], 0x00
		fstp dword ptr ds : [esp + 0x70]
		fld dword ptr ds : [esi + 0xDC]
		fmul dword ptr ds : [esi + 0xD4]
		fmul dword ptr ds : [FEReflBrightness]
		fmul dword ptr ds : [VehicleReflectionBrightness]
		fsub dword ptr ds : [esp + 0x34]
		fstp dword ptr ds : [esp + 0x74]
		fld dword ptr ds : [esi + 0xE0]
		fmul dword ptr ds : [esi + 0xD4]
		fmul dword ptr ds : [FEReflBrightness]
		fmul dword ptr ds : [VehicleReflectionBrightness]
		jmp FEReflBrightnessCodeCaveExit

	FEReflBrightnessCodeCavePart2:
		jmp FEReflBrightnessCodeCavePart2Exit
	}
}

void __declspec(naked) VehicleReflRenderDistanceCodeCave()
{
	__asm {
		mov eax, dword ptr ds : [esp + 0x6C]
		cmp dword ptr ds : [0xA99BBC], 0x06
		je VehicleReflRenderDistanceCodeCaveConditional
		mov eax, 0x481C4000

	VehicleReflRenderDistanceCodeCaveConditional:
		mov edx, dword ptr ds : [esp + 0x64]
		jmp VehicleReflRenderDistanceCodeCaveExit
	}
}

void __declspec(naked) VehicleReflFlareSizeCodeCave()
{
	__asm {
		cmp eax, 0x14
		jl VehicleReflFlareSizeCodeCaveConditional
		cmp eax, 0x1B
		jg VehicleReflFlareSizeCodeCaveConditional
		mov dword ptr ds : [esp + 0x18], 0x42B40000

	VehicleReflFlareSizeCodeCaveConditional:
		lea edx, dword ptr ds : [esp + 0x80]
		jmp VehicleReflFlareSizeCodeCaveExit
	}
}

void __declspec(naked) CubemapFixCodeCave()
{
	_asm
	{
		mov eax, 0x9D083C // "QRACE"
		mov dword ptr ds : [0x9E87E4], 0x47454E58 // ZNEG = XNEG
		mov dword ptr ds : [0x9E87EC], 0x534F5058 // ZPOS = XPOS
		mov dword ptr ds : [0x9E8804], 0x47454E5A // XNEG = ZNEG
		mov dword ptr ds : [0x9E880C], 0x534F505A // XPOS = ZPOS
		ret 0x04
	}
}

void __declspec(naked) PauseBlurCodeCave()
{
	_asm
	{
		cmp dword ptr ds : [0xA8AD18], 0x00 // checks for pause
		je originalcode
		cmp dword ptr ds : [0xA590A0], 0x1F41 // checks for photo mode
		je originalcode
		fadd dword ptr ds : [PauseBlurValue1] // 1.0f

	originalcode:
		ret
	}
}

void __declspec(naked) PauseBlurCodeCave2()
{
	_asm
	{
		cmp dword ptr ds : [0xA8AD18], 0x00 // checks for pause
		je originalcode
		cmp dword ptr ds : [0xA590A0], 0x1F41 // checks for photo mode
		je originalcode
		fadd dword ptr ds : [PauseBlurValue2] // 10.0f

	originalcode:
		ret
	}
}

void __declspec(naked) VisualTreatmentCodeCave()
{
	_asm
	{
		cmp byte ptr ds : [0xA65394], 00
		jne VisualTreatmentCodeCaveEnabled // jumps if Visual Treatment is enabled
		mov edx, dword ptr ds : [0xB43070] // No Tint
		jmp VisualTreatmentCodeCaveExit1

		VisualTreatmentCodeCaveEnabled:
		jmp VisualTreatmentCodeCaveExit2
	}
}

void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSCHDReflections.ini");

	// Resolution
	HDReflections = iniReader.ReadInteger("RESOLUTION", "HDReflections", 1);
	OldGPUCompatibility = iniReader.ReadInteger("RESOLUTION", "OldGPUCompatibility", 0);
	VehicleScale = iniReader.ReadFloat("RESOLUTION", "VehicleScale", 1.0);
	RoadScale = iniReader.ReadFloat("RESOLUTION", "RoadScale", 1.0);
	MirrorScale = iniReader.ReadFloat("RESOLUTION", "MirrorScale", 1.0);
	PIPScale = iniReader.ReadFloat("RESOLUTION", "PIPScale", 1.0);

	// General
	ImproveReflectionLOD = iniReader.ReadInteger("GENERAL", "ImproveReflectionLOD", 1);
	XB360Reflections = iniReader.ReadInteger("GENERAL", "XB360Reflections", 1);
	AlignmentFix = iniReader.ReadInteger("GENERAL", "AlignmentFix", 1);
	AspectRatioFix = iniReader.ReadInteger("GENERAL", "AspectRatioFix", 1);
	CubemapFix = iniReader.ReadInteger("GENERAL", "CubemapFix", 1);
	RestoreSkybox = iniReader.ReadInteger("GENERAL", "RestoreSkybox", 1);
	EnableMirror = iniReader.ReadInteger("GENERAL", "EnableMirror", 1);
	SynchronizeVisualTreatment = iniReader.ReadInteger("GENERAL", "SynchronizeVisualTreatment", 1);
	RealFrontEndReflections = iniReader.ReadInteger("GENERAL", "RealFrontEndReflections", 0);
	ReflectionBlurStrength = iniReader.ReadFloat("GENERAL", "ReflectionBlurStrength", 1.0f);
	VehicleReflectionBrightness = iniReader.ReadFloat("GENERAL", "VehicleReflectionBrightness", 1.0);

	// Extra
	ReplaceVisualTreatment = iniReader.ReadInteger("EXTRA", "ReplaceVisualTreatment", 0);
	TrafficSignFix = iniReader.ReadInteger("EXTRA", "TrafficSignFix", 1);
	PauseBlur = iniReader.ReadInteger("EXTRA", "PauseBlur", 0);
	RealisticChrome = iniReader.ReadInteger("EXTRA", "RealisticChrome", 0);

	if (HDReflections)
	{
		RoadResX = GetSystemMetrics(SM_CXSCREEN);
		RoadResY = GetSystemMetrics(SM_CYSCREEN);
		VehicleRes = GetSystemMetrics(SM_CYSCREEN);
		MirrorResX = GetSystemMetrics(SM_CYSCREEN);
		MirrorResY = GetSystemMetrics(SM_CYSCREEN) / 3;
		PIPRes = GetSystemMetrics(SM_CYSCREEN) / 3;
	}

	// Writes Resolution Values
	{
		// Road Reflection X
		injector::MakeJMP(0x71AA26, RoadReflectionCodeCave1, true);
		injector::MakeJMP(0x71AA76, RoadReflectionCodeCave2, true);
		injector::WriteMemory<uint32_t>(0x71BE28, RoadResX * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x71BDF1, RoadResX * RoadScale, true);
		// Road Reflection Y
		injector::MakeJMP(0x71A9F2, RoadReflectionCodeCave3, true);
		injector::WriteMemory<uint32_t>(0x71BE2F, RoadResY * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x71BDF8, RoadResY * RoadScale, true);
		// Vehicle Reflection
		injector::WriteMemory<uint32_t>(0x70DE39, VehicleRes * VehicleScale, true);
		// Rearview Mirror
		// Aspect ratio is based on NFSU2 because true aspect ratio is unknown
		injector::WriteMemory<uint32_t>(0x70DB04, MirrorResX * MirrorScale, true);
		injector::MakeNOP(0x70DB08, 2, true);
		injector::WriteMemory<uint32_t>(0x70DB62, MirrorResY * MirrorScale, true);
		injector::MakeNOP(0x70DB66, 2, true);
		// Picture-In-Picture
		injector::WriteMemory<uint32_t>(0xA63B00, PIPRes * PIPScale, true);
		injector::WriteMemory<uint32_t>(0xA63B04, PIPRes * PIPScale, true);

		if (OldGPUCompatibility)
		{
			// Rounds vehicle resolution down to the nearest power of two
			static int VehicleRes_POT = (VehicleRes * VehicleScale);
			VehicleRes_POT--;
			VehicleRes_POT |= VehicleRes_POT >> 1;
			VehicleRes_POT |= VehicleRes_POT >> 2;
			VehicleRes_POT |= VehicleRes_POT >> 4;
			VehicleRes_POT |= VehicleRes_POT >> 8;
			VehicleRes_POT |= VehicleRes_POT >> 16;
			VehicleRes_POT++;
			injector::WriteMemory<uint32_t>(0x70DE39, VehicleRes_POT, true);
		}
	}

	if (ImproveReflectionLOD >= 1)
	{
		// Partial RVM and Vehicle Reflection LOD
		injector::MakeJMP(0x79FB5A, ImproveReflectionLODCodeCave1, true);

		if (ImproveReflectionLOD >= 2)
		// Full Vehicle Reflection LOD
		injector::MakeJMP(0x79AEDB, ImproveReflectionLODCodeCave2, true);
		
		if (ImproveReflectionLOD >= 2)
		// Full RVM LOD
		injector::MakeJMP(0x79AE94, ImproveReflectionLODCodeCave3, true);
	}

	if (XB360Reflections)
	{
		// Changes the vehicle reflection shader
		injector::MakeJMP(0x729445, VehicleReflShaderCodeCave, true);
		injector::MakeJMP(0x7236CF, VehicleReflShaderSettingCodeCave, true);
		injector::MakeJMP(0x748A97, VehicleReflBrightnessCodeCave, true);
		injector::MakeJMP(0x7497DE, VehicleReflBrightnessCodeCave2, true);
		injector::MakeJMP(0x7498AA, VehicleReflSkyboxBrightnessCodeCave, true);
		// Adjusts the size of flares in the vehicle reflection
		injector::MakeJMP(0x74D9D5, VehicleReflFlareSizeCodeCave, true);
	}

	if (AlignmentFix)
	{
		// Corrects the FOV so all segments of the vehicle reflection align properly
		injector::MakeJMP(0x71B89F, AlignmentFixCodeCave, true);
		// Restores the original aspect ratio
		injector::MakeJMP(0x73E1B5, VehicleReflAspectRatioCodeCave, true);
	}
	if (AspectRatioFix)
	{
		// Restores the original aspect ratio
		injector::MakeJMP(0x73E1B5, VehicleReflAspectRatioCodeCave, true);
	}

	if (CubemapFix)
	{
		// Fixes the "QRACE" cubemap
		injector::MakeJMP(0x572C30, CubemapFixCodeCave, true);
	}

	if (RestoreSkybox)
	{
		// Increases the skybox render distance for vehicle reflections
		injector::MakeJMP(0x7AFCA1, VehicleReflSkyboxCodeCave, true);
		// Fixes skybox position for road reflections
		injector::WriteMemory<uint32_t>(0x9EF050, 0x40000000, true);
		injector::WriteMemory<uint32_t>(0x9EF05C, 0x3F800000, true);
		injector::WriteMemory<uint32_t>(0x7AFC35, 0xBF800000, true);
	}

	if (EnableMirror)
	{
		// Enables the rearview mirror by default
		injector::WriteMemory<uint32_t>(0x4BD511, 0x00000001, true);
	}

	if (SynchronizeVisualTreatment)
	{
		// Rearview Mirror
		injector::MakeCALL(0x750CAB, VisualTreatmentCodeCave, true);
		// Picture-In-Picture
		injector::MakeCALL(0x750D9F, VisualTreatmentCodeCave, true);
	}

	if (ReplaceVisualTreatment >= 1)
	{
		if (ReplaceVisualTreatment == 1)
		{
			// Purple Tint
			injector::WriteMemory<uint32_t>(0x71D702, 0xB4306C, true);
		}
		
		else
		{
			// No Tint
			injector::WriteMemory<uint32_t>(0x71D702, 0xB43070, true);
		}
	}

	if (RealFrontEndReflections)
	{
		// Enables real reflections
		injector::MakeNOP(0x72ECE8, 2, true);
		// Enables reflection mapping 
		injector::MakeNOP(0x73E19F, 2, true);
		// Enables vehicle skybox
		injector::MakeNOP(0x72E4E6, 2, true);
		// Increases render distance
		injector::MakeJMP(0x721DE3, VehicleReflRenderDistanceCodeCave, true);
		// Reduces reflection brightness if XB360Reflections = 0
		injector::MakeJMP(0x71E1A2, FEReflBrightnessCodeCave, true);
		// Allows geometry to render
		injector::MakeJMP(0x79FB5A, ImproveReflectionLODCodeCave1, true);
		injector::MakeJMP(0x79AEDB, ImproveReflectionLODCodeCave2, true);
	}

	if (TrafficSignFix)
	{
		// Restores the traffic sign brightness
		injector::MakeJMP(0x71E07F, TrafficSignFixCodeCave, true);
		// TrafficSignDistance
		injector::WriteMemory(0x79B257, &TrafficSignDistance, true);
	}

	if (PauseBlur)
	{
		injector::MakeJMP(0x73ECA6, PauseBlurCodeCave, true);
		injector::MakeJMP(0x73ECB6, PauseBlurCodeCave, true);
		injector::MakeJMP(0x73ECC6, PauseBlurCodeCave2, true);
		injector::MakeJMP(0x73ECD6, PauseBlurCodeCave2, true);
	}

	if (RealisticChrome)
	{
		// Changes Chrome Materiel values
		injector::MakeJMP(0x71E089, RealisticChromeCodeCave, true);
	}
	
	if (ReflectionBlurStrength >= 0)
	{
		// Calls
		injector::MakeCALL(0x73CC29, DownScale4x4StrengthCodeCave, true);
		injector::MakeNOP(0x73CC2E, 1, true);
		injector::MakeCALL(0x73CC3D, DownScale4x4StrengthCodeCave, true);
		injector::MakeNOP(0x73CC42, 1, true);
		// Allows blur to render at full resolution
		injector::MakeNOP(0x713108, 2, true);
		injector::MakeNOP(0x713141, 2, true);
		// TwoPassBlur
		injector::WriteMemory(0x73C97E, &ReflectionBlurStrength, true);
		// GaussBlur5x5
		injector::WriteMemory(0x73C68E, &DOFStrength, true);
		injector::WriteMemory(0x73C6A8, &DOFStrength, true);
	}

	if (VehicleReflectionBrightness)
	{
		static float VehicleReflectionIntensity = (0.5f * VehicleReflectionBrightness);
		injector::WriteMemory(0x71E1AA, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x71E1C0, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x71E1D6, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x71E1FB, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x71E231, &VehicleReflectionIntensity, true);
		injector::WriteMemory(0x71E24B, &VehicleReflectionIntensity, true);
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
