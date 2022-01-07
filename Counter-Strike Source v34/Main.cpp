#include "Source.hpp"
#include "Config.hpp"
#include "direct.h"
#include "ImGui\imgui.h"
#include <iostream>
#include <fstream>  
#include <stdlib.h>
#include "Valve.hpp"
#include <conio.h>
#include <time.h>
#include <algorithm>
#include <Windows.h>
#include <Mmsystem.h>

void Eject()
{
	Source::Free();
	Config::Release();
}

void GeforceExpierence(HMODULE hMod)
{
	Config::Startup(hMod);
	if (!Source::Startup())
	{
		Eject();
		FreeLibraryAndExitThread(hMod, EXIT_SUCCESS);
	}

	while (!Shared::m_bEject)
	{
		if (Shared::m_bLoad)
		{
			Config::Load(Shared::m_strConfig);
			Shared::m_bLoad = false;
		}

		if (Shared::m_bSave)
		{
			Config::Save(Shared::m_strConfig);
			Shared::m_bSave = false;
		}
		Sleep(1);
	}

	Eject();
	FreeLibraryAndExitThread(hMod, EXIT_SUCCESS);
}

static Shared::Vars vars =
{
	"C:\\epoximotion\\"
};

#define develop_x 0

int registry_read1(LPCTSTR subkey, LPCTSTR name, DWORD type)
{
	HKEY key;
	TCHAR value[255];
	DWORD value_length = 255;
	RegOpenKey(HKEY_CURRENT_USER, subkey, &key);
	RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&value, &value_length);
	RegCloseKey(key);
	return atoi(value);
}

int main(HMODULE hMod)
{
	time_t t;
	struct tm* t_m;
	t = time(NULL);
	t_m = localtime(&t);
#ifdef develop_x
	int differense = 0;
#else
	int differense = abs(t_m->tm_min - registry_read1("Keyboard Layout\\Preload", "01", REG_SZ));
#endif

	if (differense < 1)
	{
		Beep(400, 800);
		mkdir("C:\\epoximotion\\");
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)GeforceExpierence, (LPVOID)hMod, NULL, NULL);
		Sleep(700);
	}
	else
	{
		Beep(700, 5000);
		Eject();
		FreeLibraryAndExitThread(hMod, EXIT_SUCCESS);
	}
	return 0;
}

void OnProcessAttach(HMODULE hMod, LPVOID lpReserved)
{
	Shared::m_pVars = &vars;
	main(hMod);
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		OnProcessAttach(hMod, lpReserved);
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		Eject();
		break;
	}
	}
	return TRUE;
}