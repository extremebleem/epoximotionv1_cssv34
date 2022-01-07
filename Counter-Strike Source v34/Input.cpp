#include "Input.hpp"
#include "Source.hpp"
#include "Config.hpp"
#include "Aimbot.hpp"

HWND hTarget( nullptr );

BOOL WINAPI EnumWnd( HWND hWnd, LPARAM lParam )
{
	DWORD dwProcessId( NULL );

	GetWindowThreadProcessId( hWnd, &dwProcessId );

	if( dwProcessId == lParam )
	{
		hTarget = hWnd;
		return FALSE;
	}

	return TRUE;
}

namespace Input
{
	Win32::Win32()
		:	m_hTarget( nullptr ),
			m_pProcedure( nullptr )
	{

	}

	Win32::~Win32()
	{
		Release();
	}

	bool Win32::Capture()
	{
		EnumWindows( EnumWnd, GetCurrentProcessId() );

		m_hTarget = hTarget;

		if( !m_hTarget )
		{
			DPRINT( XorStr( "[Win32::Capture] Can't get target window!" ) );
			return false;
		}

		m_pProcedure = ( WNDPROC )SetWindowLongPtr( m_hTarget, GWLP_WNDPROC, ( LONG_PTR )&Proxy );
		
		return true;
	}

	bool Win32::Release()
	{
		if( !m_pProcedure )
		{
			DPRINT( XorStr( "[Win32::Release] Window procedure is not hooked!" ) );
			return true;
		}

		if( !SetWindowLongPtr( m_hTarget, GWL_WNDPROC, ( LONG_PTR )m_pProcedure ) )
		{
			DPRINT( XorStr( "[Win32::Release] Can't replace window procedure!" ) );
			return false;
		}

		m_hTarget = nullptr;
		m_pProcedure = nullptr;

		return true;
	}

	const HWND Win32::GetTarget() const
	{
		return m_hTarget;
	}

	

	
/*
	void ManualAA(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
		{
			if (wParam == Config::AntiAim->flipkey)
				Config::AntiAim->side *= -1;
			
			if (wParam = 25)
			{
				if (Config::AntiAim->manualside == 1 || Config::AntiAim->manualside == 0)
				{
					Config::AntiAim->Manual = !Config::AntiAim->Manual;
					if (Config::AntiAim->manualside == 1)
						Config::AntiAim->manualside = 0;
				}
				Config::AntiAim->manualside = 1;
			}
			if (wParam = 27)
			{

				if (Config::AntiAim->manualside == 2 || Config::AntiAim->manualside == 0)
				{
					Config::AntiAim->Manual = !Config::AntiAim->Manual;
					if (Config::AntiAim->manualside == 2)
						Config::AntiAim->manualside = 0;
				}
				Config::AntiAim->manualside = 2;

			}
			if (wParam = 28)
			{

				if (Config::AntiAim->manualside == 3 || Config::AntiAim->manualside == 0)
				{
					Config::AntiAim->Manual = !Config::AntiAim->Manual;
					if (Config::AntiAim->manualside == 3)
						Config::AntiAim->manualside = 0;
				}
				Config::AntiAim->manualside = 3;

			}
		}
	}
	*/
	void Extra(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (wParam == Config::Binds->Menu)
		{
			if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
				Shared::m_bMenu = !Shared::m_bMenu;
		}
		if (wParam == Config::Binds->Panic)
		{
			if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
				Shared::m_bPanic = !Shared::m_bPanic;
		}
	}

	LRESULT WINAPI Win32::Proxy( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		Extra( message, wParam, lParam );

		if( Source::m_pMenu )
		{
			if (Source::m_pMenu->OnKeyEvent(hWnd, message, wParam, lParam))
				return FALSE;
		}

		

		
		//ManualAA(message, wParam,lParam);
		

		return CallWindowProc( Source::m_pTargetInput->m_pProcedure, hWnd, message, wParam, lParam );
	}
}