#pragma once

#include <d3d9.h>

enum
{
	ENEMY_COLOR,
	ENEMY_COLOR_WALL,
	ENEMY_COLOR_OUTLINE,
	TEAM_COLOR,
	TEAM_COLOR_WALL,
	TEAM_COLOR_OUTLINE,
	LOCAL_COLOR,
	LOCAL_COLOR_OUTLINE,
	WEAPON_COLOR,
	WEAPON_COLOR_WALL,
	WEAPON_COLOR_OUTLINE
};

namespace Feature
{
	class Menu
	{
	public:
		Menu();
		~Menu();

		bool	Create( HWND hWnd, IDirect3DDevice9* pDevice );

	
		void	OnPresentDevice();
		
		bool	OnKeyEvent( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		void	OnLostDevice();
		void	OnResetDevice();

	

		void	SetColors();
		void	ApplyColors();
		

	private:
		bool	m_bMouse;

		int		m_iWeaponAimbot;
		int		m_iWeaponTriggerbot;

		int		m_iConfig;
		bool	m_bConfigSelected[ 1024 ];
		char	m_szConfigName[ 256 ];

		bool	m_bAimbot;
		bool	m_bTriggerbot;
		bool	m_bESP;
		bool	m_bAntiAim;
		bool	m_bRemovals;
		bool	m_bMisc;
		bool	m_bPlayers;
		bool	m_bColors;
		bool	m_bBinds;

		bool	m_bConfig;
		float	m_flColors[32][4];
	};
}