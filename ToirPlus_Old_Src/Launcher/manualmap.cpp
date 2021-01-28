#include "stdafx.h"
////////////////////////////////////////////////////////////////////////////////////////////
// loader: command-line interface dll injector
// Copyright (C) 2009-2011 Wadim E. <wdmegrv@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------------------------------------------------------

#include "manualmap.h"


//-------------------------------------------------------------------------------------------------------------------
// Matt Pietrek's function
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD_PTR rva, PIMAGE_NT_HEADERS pNTHeader)
{
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
	WORD nSection = 0;

	for(nSection = 0; nSection < pNTHeader->FileHeader.NumberOfSections; nSection++, section++ )
	{
		// This 3 line idiocy is because Watcom's linker actually sets the
		// Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
		DWORD_PTR size = section->Misc.VirtualSize;
		if(size == 0)
		{
			size = section->SizeOfRawData;
		}

		// Is the RVA within this section?
		if( (rva >= section->VirtualAddress) && (rva < (section->VirtualAddress + size)) )
		{
			return section;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// Matt Pietrek's function
LPVOID GetPtrFromRVA(DWORD_PTR rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase)
{
	PIMAGE_SECTION_HEADER section;
	LONG_PTR delta;

	section = GetEnclosingSectionHeader(rva, pNTHeader);
	if(!section)
	{
		return 0;
	}

	delta = (LONG_PTR)( section->VirtualAddress - section->PointerToRawData );
	return (LPVOID)( imageBase + rva - delta );
}

//-------------------------------------------------------------------------------------------------------------------
DWORD nBase_MSVC_R   = 0;
DWORD nBase_MSVC_P   = 0;
DWORD nBase_WINHTTP  = 0;// disc vi Laucher ko co dll-import winhttp.dll
BOOL FixIAT(DWORD dwProcessId, HANDLE hProcess, PBYTE imageBase, PIMAGE_NT_HEADERS pNtHeader, PIMAGE_IMPORT_DESCRIPTOR pImgImpDesc)
{
	VMProtectBegin("Launcer_VM__FixIAT");
	BOOL bRet = FALSE;
	LPSTR lpModuleName = 0;
	HMODULE hLocalModule = 0;
	HMODULE hRemoteModule = 0;
	WCHAR modulePath[MAX_PATH + 1] = {0};
	WCHAR moduleNtPath[500 + 1] = {0};
	WCHAR targetProcPath[MAX_PATH + 1] = {0};
	WCHAR *pch = 0;

	__try
	{
		//__oMsg("Fixing Imports:\n");

		////// get target process path
		////if(!GetModuleFileNameExW(hProcess, (HMODULE)0, targetProcPath, MAX_PATH))
		////{
		////	__oMsg("Could not get path to target process.");
		////	__leave;
		////}

		////pch = wcsrchr(targetProcPath, '\\');
		////if(pch)
		////{
		////	targetProcPath[ pch - targetProcPath + 1 ] = (WCHAR)0;
		////}

		////if(!SetDllDirectoryW(targetProcPath))
		////{
		////	__oMsg("Could not set path to target process (%s).", targetProcPath);
		////	__leave;
		////}

		while((lpModuleName = (LPSTR)GetPtrFromRVA(pImgImpDesc->Name, pNtHeader, imageBase)))
		{
			PIMAGE_THUNK_DATA itd = 0;

			__oMsg("------ MODULE --: %s\n", lpModuleName);

			// ACHTUNG: LoadLibraryEx kann eine DLL nur anhand des Namen aus einem anderen
			// Verzeichnis laden wie der Zielprozess!
			hLocalModule = LoadLibraryExA(lpModuleName, 0, DONT_RESOLVE_DLL_REFERENCES);
			if(!hLocalModule)
			{
				//__oMsg("Could not load module locally.");
				__leave;
			}

			// get full path of module
			if(!GetModuleFileNameW(hLocalModule, modulePath, MAX_PATH))
			{
				//__oMsg("Could not get path to module (%s).", lpModuleName);
				__leave;
			}

			// get nt path
			if(!GetFileNameNtW(modulePath, moduleNtPath, 500))
			{
				//__oMsg("Could not get the NT namespace path.");
				__leave;
			}

			// Module already in process?
			hRemoteModule = (HMODULE)ModuleInjectedW(hProcess, moduleNtPath);
			//__oMsg("[=1=]hRemoteModule :[%x]: lpModuleName[%s],  NtPath(%s), modulePath: %s.\n", hRemoteModule, lpModuleName, W2C(moduleNtPath), W2C(modulePath));

			if(strcmp(lpModuleName, "msvcr90.dll") == 0 || strcmp(lpModuleName, "MSVCR90.dll") == 0)
			{
				if(hRemoteModule == 0)
				{
					hRemoteModule = (HMODULE)MapRemoteModuleW_DLL_Import(dwProcessId, modulePath);
					nBase_MSVC_R  = (DWORD)hRemoteModule;
					//__oMsg("XONG XONG XONG - msvc-RRRR-90: %x ", hRemoteModule);
					if(nBase_MSVC_R != 0)// Erase PE-Header: MSVCR90
					{
						DWORD nSize  = 4;
						DWORD nValue = 0;
						DWORD nData     = 0;
						for (int i = 0; i < 1024; ++i)
						{
							nData = nBase_MSVC_R + 4*i;
							WriteProcessMemory(hProcess, (LPVOID)nData, &nValue, nSize, &nSize);
						}
					}
				}
			}
			if(strcmp(lpModuleName, "msvcp90.dll") == 0 || strcmp(lpModuleName, "MSVCP90.dll") == 0)
			{
				if(hRemoteModule == 0)
				{
					hRemoteModule = (HMODULE)MapRemoteModuleW_DLL_Import(dwProcessId, modulePath);
					nBase_MSVC_P  = (DWORD)hRemoteModule;
					//__oMsg("XONG XONG XONG - msvc-PPPP-90: %x", hRemoteModule);
					if(nBase_MSVC_P != 0)// Erase PE-Header: MSVCP90
					{
						DWORD nSize  = 4;
						DWORD nValue = 0;
						DWORD nData     = 0;
						for (int i = 0; i < 1024; ++i)
						{
							nData = nBase_MSVC_P + 4*i;
							WriteProcessMemory(hProcess, (LPVOID)nData, &nValue, nSize, &nSize);
						}
					}
				}
			}
			if(!hRemoteModule)
			{			
				if(!InjectLibraryW(dwProcessId, modulePath))
				{
					//__oMsg("ERROR==> Could not inject required module (%s).\n", W2C(modulePath));
					__leave;
				}
				
				hRemoteModule = (HMODULE)ModuleInjectedW(hProcess, moduleNtPath);
			}

			itd = (PIMAGE_THUNK_DATA)GetPtrFromRVA(pImgImpDesc->FirstThunk, pNtHeader, imageBase);
			while(itd->u1.AddressOfData)
			{
				IMAGE_IMPORT_BY_NAME *iibn =
					(PIMAGE_IMPORT_BY_NAME)GetPtrFromRVA(itd->u1.AddressOfData, pNtHeader, imageBase);
				itd->u1.Function = (DWORD_PTR)GetRemoteProcAddress(hProcess, hRemoteModule, (LPCSTR)iibn->Name);

				//__oMsg("Function: %s\n", (LPCSTR)iibn->Name);

				itd++;
			}      

			pImgImpDesc++;
		}

		bRet = TRUE;
	}
	__finally
	{
		if(hLocalModule)
		{
			FreeLibrary(hLocalModule);
		}
	}
	VMProtectEnd();
	return bRet;
}

//-------------------------------------------------------------------------------------------------------------------
BOOL MapSections(HANDLE hProcess, LPVOID lpModuleBase, PBYTE dllBin, PIMAGE_NT_HEADERS pNTHeader)
{
	VMProtectBegin("Launcer_VM__MapSections");
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
	SIZE_T virtualSize = 0;
	WORD nSection = 0;
	
	for(nSection = 0; nSection < pNTHeader->FileHeader.NumberOfSections; nSection++)
	{
		LPVOID lpBaseAddress = (LPVOID)( (DWORD_PTR)lpModuleBase + section->VirtualAddress );
		LPCVOID lpBuffer = (LPCVOID)( (DWORD_PTR)dllBin + section->PointerToRawData );
		SIZE_T NumBytesWritten = 0;
		PDWORD lpflOldProtect = 0;

		if(!WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, section->SizeOfRawData, &NumBytesWritten) ||
			NumBytesWritten != section->SizeOfRawData)
		{
			//__oMsg("Could not write to memory in remote process.");
			return FALSE;
		}	
		
		// next section header, calculate virtualSize of section header
		virtualSize = section->VirtualAddress;
		//__oMsg("section: %s | %p | %x\n", section->Name, section->VirtualAddress, virtualSize);
		section++;
		if(section->VirtualAddress)
		{
			virtualSize = section->VirtualAddress - virtualSize;
		}
		/*
		if(!VirtualProtectEx(hProcess, (LPVOID)( (DWORD_PTR)lpModuleBase + section->VirtualAddress ), virtualSize,
			section->Characteristics & 0x00FFFFFF, lpflOldProtect))
		{
			__oMsg("VirtualProtectEx failed.");
			return FALSE;
		}
		*/
	}
	VMProtectEnd();
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------
BOOL FixRelocations(PBYTE dllBin, LPVOID lpModuleBase, PIMAGE_NT_HEADERS pNtHeader, PIMAGE_BASE_RELOCATION pImgBaseReloc)
{
	LONG_PTR delta = (DWORD_PTR)lpModuleBase - pNtHeader->OptionalHeader.ImageBase;
	SIZE_T relocationSize = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
	WORD *pRelocData = 0;

	//__oMsg("FixRelocs:\n");

	// image has no relocations
	if(!pImgBaseReloc->SizeOfBlock)
	{
		//__oMsg("Image has no relocations\n");
		return TRUE;
	}
	
	do
	{
		PBYTE pRelocBase = (PBYTE)GetPtrFromRVA(pImgBaseReloc->VirtualAddress, pNtHeader, dllBin);
		SIZE_T numRelocations = (pImgBaseReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
		SIZE_T i = 0;

		//__oMsg("numRelocations: %d\n", numRelocations);

		pRelocData = (WORD*)( (DWORD_PTR)pImgBaseReloc + sizeof(IMAGE_BASE_RELOCATION) );

		// loop over all relocation entries
		for(i = 0; i < numRelocations; i++, pRelocData++)
		{
			// Get reloc data
			BYTE RelocType = *pRelocData >> 12;
			WORD Offset = *pRelocData & 0xFFF;

			switch(RelocType)
			{
			case IMAGE_REL_BASED_ABSOLUTE:
				break;

			case IMAGE_REL_BASED_HIGHLOW:
				*(DWORD32*)(pRelocBase + Offset) += (DWORD32)delta;
				break;

			case IMAGE_REL_BASED_DIR64:
				*(DWORD64*)(pRelocBase + Offset) += delta;

				break;

			default:
				//__oMsg("Unsuppported relocation type.");
				return FALSE;
			}
		}

		pImgBaseReloc = (PIMAGE_BASE_RELOCATION)pRelocData;

	} while( *(DWORD*)pRelocData );

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------
BOOL CallTlsInitializers(PBYTE imageBase, PIMAGE_NT_HEADERS pNtHeader, HANDLE hProcess, HMODULE hModule, DWORD fdwReason, PIMAGE_TLS_DIRECTORY pImgTlsDir)
{
	DWORD_PTR pCallbacks = (DWORD_PTR)pImgTlsDir->AddressOfCallBacks;

	if(pCallbacks)
	{
		while(TRUE)
		{
			SIZE_T NumBytesRead = 0;
			LPVOID callback = 0;

			if(!ReadProcessMemory(hProcess, (PVOID)pCallbacks, &callback, sizeof(LPVOID), &NumBytesRead) ||
				NumBytesRead != sizeof(LPVOID))
			{
				//__oMsg("Could not read memory in remote process.");
				return FALSE;
			}

			if(!callback) break;

			RemoteDllMainCall(hProcess, callback, hModule, fdwReason, 0);
			//__oMsg("callback: %p\n", callback);
			pCallbacks += sizeof(DWORD_PTR);
		}
	}
	return TRUE;
}

typedef void (*init_fakeProc)();
//-------------------------------------------------------------------------------------------------------------------
BOOL MapRemoteModuleW(DWORD dwProcessId, LPCWSTR lpModulePath)
{
	BOOL bRet = FALSE;
	HANDLE hFile = 0;
	DWORD fileSize = 0;
	BYTE *dllBin = 0;
	PIMAGE_NT_HEADERS nt_header = 0;
	PIMAGE_DOS_HEADER dos_header = 0;
	HANDLE hProcess = 0;
	LPVOID lpModuleBase = 0;

	PIMAGE_IMPORT_DESCRIPTOR pImgImpDesc = 0;
	PIMAGE_BASE_RELOCATION pImgBaseReloc = 0;
	PIMAGE_TLS_DIRECTORY pImgTlsDir = 0;

	__try
	{
		// Get a handle for the target process.
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ, FALSE, dwProcessId);
		if(!hProcess)
		{
			__oMsg("Could not get handle to process (PID: 0x%X).", dwProcessId);
			__leave;
		}

		hFile = CreateFileW(lpModulePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			__oMsg("CreateFileW failed.");
			__leave;
		}

		if(GetFileAttributesW(lpModulePath) & FILE_ATTRIBUTE_COMPRESSED)
		{
			fileSize = GetCompressedFileSizeW(lpModulePath, NULL);
		}
		else
		{
			fileSize = GetFileSize(hFile, NULL);
		}

		if(fileSize == INVALID_FILE_SIZE)
		{
			//__oMsg("Could not get size of file.");
			__leave;
		}

		dllBin = (BYTE*)malloc(fileSize);

		{
			DWORD NumBytesRead = 0;
			if(!ReadFile(hFile, dllBin, fileSize, &NumBytesRead, FALSE))
			{
				//__oMsg("ReadFile failed.");
			}
		}
	
		dos_header = (PIMAGE_DOS_HEADER)dllBin;
		
		// Make sure we got a valid DOS header
		if(dos_header->e_magic != IMAGE_DOS_SIGNATURE)
		{
			//__oMsg("Invalid DOS header.");
			__leave;
		}
		
		// Get the real PE header from the DOS stub header
		nt_header = (PIMAGE_NT_HEADERS)((DWORD_PTR)dllBin + dos_header->e_lfanew);

		// Verify the PE header
		if(nt_header->Signature != IMAGE_NT_SIGNATURE)
		{
			//__oMsg("Invalid PE header.");
			__leave;
		}

		// Allocate space for the module in the remote process
		DWORD OrgProtection;
		lpModuleBase = VirtualAllocEx(hProcess, NULL, nt_header->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		
		if(!lpModuleBase)
		{
			__oMsg("Could not allocate memory in remote process.");
			__leave;
		}
		//__oMsg("@Fixing imports.");
		// fix imports
		pImgImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)GetPtrFromRVA(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
		{
			if(!FixIAT(dwProcessId, hProcess, (PBYTE)dllBin, nt_header, pImgImpDesc))
			{
				__oMsg("@Fixing imports.");
				__leave;
			}
		}
		
		// fix relocs
		pImgBaseReloc = (PIMAGE_BASE_RELOCATION)GetPtrFromRVA((DWORD)(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress), nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
		{
			if(!FixRelocations(dllBin, lpModuleBase, nt_header, pImgBaseReloc))
			{
				__oMsg("@Fixing relocations.");
				__leave;
			}
		}
		//__oMsg("@Fixing relocations.");
		// Write the PE header into the remote process's memory space
		{
			SIZE_T NumBytesWritten = 0;
			SIZE_T nSize = nt_header->FileHeader.SizeOfOptionalHeader + sizeof(nt_header->FileHeader) + sizeof(nt_header->Signature);
			
			if(!WriteProcessMemory(hProcess, lpModuleBase, dllBin, nSize, &NumBytesWritten) ||
				NumBytesWritten != nSize)
			{
				//__oMsg("Could not write to memory in remote process.");
				__leave;
			}
		}

		// Map the sections into the remote process(they need to be aligned along their virtual addresses)
		if(!MapSections(hProcess, lpModuleBase, dllBin, nt_header))
		{
			__oMsg("@Map sections.");
			__leave;
		}

		// call all tls callbacks
		pImgTlsDir = (PIMAGE_TLS_DIRECTORY)GetPtrFromRVA(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress, nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
		{
			//__oMsg("CallTlsInitializers");
			//if(!CallTlsInitializers(dllBin, nt_header, hProcess, (HMODULE)lpModuleBase, DLL_PROCESS_ATTACH, pImgTlsDir))
			//{
			//	//__oMsg("@Call TLS initializers.");
			//	__leave;
			//}
		}

		// call entry point
		//if(!RemoteDllMainCall(hProcess, (LPVOID)( (DWORD_PTR)lpModuleBase + nt_header->OptionalHeader.AddressOfEntryPoint), (HMODULE)lpModuleBase, 1, 0))
		//{
		//	//__oMsg("@Call DllMain.");
		//	__leave;
		//}

//------------------------------------------
		//viet code o day

		init_fakeProc fake;
		__oMsg("init_fakeProc-1: %x", lpModuleBase);
		fake = (init_fakeProc)((DWORD)lpModuleBase + 0x10B0);//0x33A0 
		__oMsg("init_fakeProc-2: %x --> %x", fake, (DWORD)lpModuleBase + 0x10B0);

		DWORD dwThreadId;
		CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)fake, NULL, 0, &dwThreadId);
//------------------------------------------
		VirtualProtectEx(hProcess, (LPVOID)lpModuleBase, nt_header->OptionalHeader.SizeOfImage, PAGE_READWRITE, &OrgProtection);
		bRet = TRUE;

		__oMsg("Successfully injected [dll-MAIN] (%s | PID: %x):\n\n"
			"  AllocationBase:\t0x%p\n"
			"  EntryPoint:\t\t0x%p\n"
			"  SizeOfImage:\t\t0x%p\n"
			"  CheckSum:\t\t0x%p\n",
			W2C(lpModulePath),
			dwProcessId,
			lpModuleBase,
			(DWORD_PTR)lpModuleBase + nt_header->OptionalHeader.AddressOfEntryPoint,
			nt_header->OptionalHeader.SizeOfImage,
			nt_header->OptionalHeader.CheckSum);
	}
	__finally
	{
		if(hFile)		CloseHandle(hFile);
		if(dllBin)		free(dllBin);
		if(hProcess)	CloseHandle(hProcess);
	}
	
	return bRet;
}

//-------------------------------------------------------------------------------------------------------------------
DWORD nBase_MainDLL  = 0;
extern int nSize_DLL;
BOOL MapRemoteModuleW_Bin(DWORD dwProcessId, unsigned char* dll)
{
	VMProtectBegin("Launcer_VM__MapRemoteModuleW_Bin");
	BOOL bRet = FALSE;
	HANDLE hFile = 0;
	DWORD fileSize = 0;
	BYTE *dllBin = 0;
	PIMAGE_NT_HEADERS nt_header = 0;
	PIMAGE_DOS_HEADER dos_header = 0;
	HANDLE hProcess = 0;
	LPVOID lpModuleBase = 0;

	PIMAGE_IMPORT_DESCRIPTOR pImgImpDesc = 0;
	PIMAGE_BASE_RELOCATION pImgBaseReloc = 0;
	PIMAGE_TLS_DIRECTORY pImgTlsDir = 0;

	__try
	{
		// Get a handle for the target process.
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ, FALSE, dwProcessId);
		if(!hProcess)
		{
			//__oMsg("Could not get handle to process (PID: 0x%X).", dwProcessId);
			__leave;
		}

		//fileSize = DLL_SIZE;
		fileSize = nSize_DLL;
		if(fileSize == INVALID_FILE_SIZE)
		{
			//__oMsg("Could not get size of file.");
			__leave;
		}
		dllBin = (BYTE*)malloc(fileSize);

		//{
		//	DWORD NumBytesRead = 0;
		//	if(!ReadFile(hFile, dllBin, fileSize, &NumBytesRead, FALSE))
		//	{
		//		__oMsg("ReadFile failed.");
		//	}
		//}

		memcpy(dllBin,dll,fileSize);
		dos_header = (PIMAGE_DOS_HEADER)dllBin;

		// Make sure we got a valid DOS header
		if(dos_header->e_magic != IMAGE_DOS_SIGNATURE)
		{
			//__oMsg("Invalid DOS header.");
			__leave;
		}

		// Get the real PE header from the DOS stub header
		nt_header = (PIMAGE_NT_HEADERS)( (DWORD_PTR)dllBin + dos_header->e_lfanew);

		// Verify the PE header
		if(nt_header->Signature != IMAGE_NT_SIGNATURE)
		{
			//__oMsg("Invalid PE header.");
			__leave;
		}

		// Allocate space for the module in the remote process
		lpModuleBase = VirtualAllocEx(hProcess, NULL, nt_header->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if(!lpModuleBase)
		{
			//__oMsg("Could not allocate memory in remote process.");
			__leave;
		}

		// fix imports
		pImgImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)GetPtrFromRVA(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
		{
			if(!FixIAT(dwProcessId, hProcess, (PBYTE)dllBin, nt_header, pImgImpDesc))
			{
				//__oMsg("@Fixing imports.");
				__leave;
			}
		}

		// fix relocs
		pImgBaseReloc = (PIMAGE_BASE_RELOCATION)GetPtrFromRVA((DWORD)(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress), nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
		{
			if(!FixRelocations(dllBin, lpModuleBase, nt_header, pImgBaseReloc))
			{
				//__oMsg("@Fixing relocations.");
				__leave;
			}
		}

		// Write the PE header into the remote process's memory space
		{
			SIZE_T NumBytesWritten = 0;
			SIZE_T nSize = nt_header->FileHeader.SizeOfOptionalHeader + sizeof(nt_header->FileHeader) + sizeof(nt_header->Signature);
			if(!WriteProcessMemory(hProcess, lpModuleBase, dllBin, nSize, &NumBytesWritten) || NumBytesWritten != nSize)
			{
				//__oMsg("Could not write to memory in remote process.");
				__leave;
			}
		}

		// Map the sections into the remote process(they need to be aligned along their virtual addresses)
		if(!MapSections(hProcess, lpModuleBase, dllBin, nt_header))
		{
			//__oMsg("@Map sections.");
			__leave;
		}

		// call all tls callbacks
		//
		pImgTlsDir = (PIMAGE_TLS_DIRECTORY)GetPtrFromRVA(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress, nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
		{
			//if(!CallTlsInitializers(dllBin, nt_header, hProcess, (HMODULE)lpModuleBase, DLL_PROCESS_ATTACH, pImgTlsDir))
			//{
			//	//__oMsg("@Call TLS initializers.");
			//	__leave;
			//}
		}

		//__oMsg("AddressOfEntryPoint %p", nt_header->OptionalHeader.AddressOfEntryPoint);
		//// call entry point
		//if(!RemoteDllMainCall(hProcess, (LPVOID)( (DWORD_PTR)lpModuleBase + nt_header->OptionalHeader.AddressOfEntryPoint), (HMODULE)lpModuleBase, 1, 0))
		//{
		//	//__oMsg("@Call DllMain.");
		//	__leave;
		//}
		
//------------------------------------------
		//viet code o day

		init_fakeProc fake;
		//__oMsg("init_fakeProc-1: %x", lpModuleBase);
		fake = (init_fakeProc)((DWORD)lpModuleBase + 0x34E0);
		//__oMsg("init_fakeProc-2: %x --> %x", fake, (DWORD)lpModuleBase + 0x34E0);

		DWORD dwThreadId;
		CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)fake, NULL, 0, &dwThreadId);
		DWORD OrgProtection;
		VirtualProtectEx(hProcess, (LPVOID)lpModuleBase, nt_header->OptionalHeader.SizeOfImage, PAGE_READWRITE, &OrgProtection);
//------------------------------------------
		bRet = TRUE;
		nBase_MainDLL = (DWORD)lpModuleBase;

		/*__oMsg("Successfully injected [dll-MAIN] (%s | PID: %x):\n\n"
			"  AllocationBase:\t0x%p\n"
			"  EntryPoint:\t\t0x%p\n"
			"  SizeOfImage:\t\t0x%p\n"
			"  CheckSum:\t\t0x%p\n",
			"",
			dwProcessId,
			lpModuleBase,
			(DWORD_PTR)lpModuleBase + nt_header->OptionalHeader.AddressOfEntryPoint,
			nt_header->OptionalHeader.SizeOfImage,
			nt_header->OptionalHeader.CheckSum);*/

	}
	__finally
	{
		if(hFile)		CloseHandle(hFile);
		if(dllBin)		free(dllBin);
		if(hProcess)	CloseHandle(hProcess);
	}
	VMProtectEnd();
	return bRet;
}

//-------------------------------------------------------------------------------------------------------------------
DWORD MapRemoteModuleW_DLL_Import(DWORD dwProcessId, LPCWSTR lpModulePath)
{
	VMProtectBegin("Launcer_VM__MapRemoteModuleW_DLL_Import");
	DWORD nRet = 0;
	HANDLE hFile = 0;
	DWORD fileSize = 0;
	BYTE *dllBin = 0;
	PIMAGE_NT_HEADERS nt_header = 0;
	PIMAGE_DOS_HEADER dos_header = 0;
	HANDLE hProcess = 0;
	LPVOID lpModuleBase = 0;

	PIMAGE_IMPORT_DESCRIPTOR pImgImpDesc = 0;
	PIMAGE_BASE_RELOCATION pImgBaseReloc = 0;
	PIMAGE_TLS_DIRECTORY pImgTlsDir = 0;

	__try
	{
		// Get a handle for the target process.
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ, FALSE, dwProcessId);
		if(!hProcess)
		{
			//__oMsg("Could not get handle to process (PID: 0x%X).", dwProcessId);
			return 0;
		}

		hFile = CreateFileW(lpModulePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			//__oMsg("CreateFileW failed.");
			return 0;
		}

		if(GetFileAttributesW(lpModulePath) & FILE_ATTRIBUTE_COMPRESSED)
		{
			fileSize = GetCompressedFileSizeW(lpModulePath, NULL);
		}
		else
		{
			fileSize = GetFileSize(hFile, NULL);
		}

		if(fileSize == INVALID_FILE_SIZE)
		{
			//__oMsg("Could not get size of file.");
			return 0;
		}
		
		dllBin = (BYTE*)malloc(fileSize);

		{
			DWORD NumBytesRead = 0;
			if(!ReadFile(hFile, dllBin, fileSize, &NumBytesRead, FALSE))
			{
				//__oMsg("ReadFile failed.");
			}
		}
		dos_header = (PIMAGE_DOS_HEADER)dllBin;
		
		// Make sure we got a valid DOS header
		if(dos_header->e_magic != IMAGE_DOS_SIGNATURE)
		{
			//__oMsg("Invalid DOS header.");
			return 0;
		}
		
		// Get the real PE header from the DOS stub header
		nt_header = (PIMAGE_NT_HEADERS)( (DWORD_PTR)dllBin + dos_header->e_lfanew);

		// Verify the PE header
		if(nt_header->Signature != IMAGE_NT_SIGNATURE)
		{
			//__oMsg("Invalid PE header.");
			return 0;
		}

		// Allocate space for the module in the remote process
		lpModuleBase = VirtualAllocEx(hProcess, NULL, nt_header->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if(!lpModuleBase)
		{
			//__oMsg("Could not allocate memory in remote process.");
			return 0;
		}
		
		// fix imports
		pImgImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)GetPtrFromRVA(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
		{
			if(!FixIAT(dwProcessId, hProcess, (PBYTE)dllBin, nt_header, pImgImpDesc))
			{
				//__oMsg("@Fixing imports.");
				return 0;
			}
		}
		
		// fix relocs
		pImgBaseReloc = (PIMAGE_BASE_RELOCATION)GetPtrFromRVA((DWORD)(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress), nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
		{
			if(!FixRelocations(dllBin, lpModuleBase, nt_header, pImgBaseReloc))
			{
				//__oMsg("@Fixing relocations.");
				return 0;
			}
		}

		// Write the PE header into the remote process's memory space
		{
			SIZE_T NumBytesWritten = 0;
			SIZE_T nSize = nt_header->FileHeader.SizeOfOptionalHeader + sizeof(nt_header->FileHeader) + sizeof(nt_header->Signature);
			if(!WriteProcessMemory(hProcess, lpModuleBase, dllBin, nSize, &NumBytesWritten) ||
				NumBytesWritten != nSize)
			{
				//__oMsg("Could not write to memory in remote process.");
				return 0;
			}
		}

		if(!MapSections(hProcess, lpModuleBase, dllBin, nt_header))
		{
			//__oMsg("@Map sections.");
			return 0;
		}

		// call all tls callbacks
		pImgTlsDir = (PIMAGE_TLS_DIRECTORY)GetPtrFromRVA(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress, nt_header, (PBYTE)dllBin);
		if(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
		{
			if(!CallTlsInitializers(dllBin, nt_header, hProcess, (HMODULE)lpModuleBase, DLL_PROCESS_ATTACH, pImgTlsDir))
			{
				//__oMsg("@Call TLS initializers.");
				return 0;
			}
		}

		////-----call entry point
		if(!RemoteDllMainCall(hProcess, (LPVOID)( (DWORD_PTR)lpModuleBase + nt_header->OptionalHeader.AddressOfEntryPoint), (HMODULE)lpModuleBase, 1, 0))
		{
			//__oMsg("@Call DllMain.");
			return 0;
		}
		nRet = (DWORD)lpModuleBase;

		/*__oMsg("Successfully injected [dll-IMPORT-xxx] (%s | PID: %x):\n\n"
			"  AllocationBase:\t0x%p\n"
			"  EntryPoint:\t\t0x%p\n"
			"  SizeOfImage:\t\t0x%p\n"
			"  CheckSum:\t\t0x%p\n",
			W2C(lpModulePath),
			dwProcessId,
			lpModuleBase,
			(DWORD_PTR)lpModuleBase + nt_header->OptionalHeader.AddressOfEntryPoint,
			nt_header->OptionalHeader.SizeOfImage,
			nt_header->OptionalHeader.CheckSum);*/
	}
	__finally
	{
		if(hFile)		CloseHandle(hFile);
		if(dllBin)		free(dllBin);
		if(hProcess)	CloseHandle(hProcess);
	}
	VMProtectEnd();
	return nRet;
}

//-------------------------------------------------------------------------------------------------------------------
BOOL MapRemoteModuleA_Bin(DWORD dwProcessId, unsigned char* dll)
{
	BOOL bRet = FALSE;
	bRet = MapRemoteModuleW_Bin(dwProcessId, dll);
	return bRet;
}

BOOL MapRemoteModuleA(DWORD dwProcessId, LPCSTR lpModulePath)
{
	BOOL bRet = FALSE;
	wchar_t *libpath = char_to_wchar_t(lpModulePath);
	if(libpath == 0)
	{
		return bRet;
	}
	bRet = MapRemoteModuleW(dwProcessId, libpath);
	if(libpath)
	{
		free(libpath);
	}
	return bRet;
}

//-------------------------------------------------------------------------------------------------------------------
