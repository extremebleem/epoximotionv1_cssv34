#pragma once

#include "Valve.hpp"

#define LimitValue( val, min, max )	if( val < min ) val = min; if( val > max ) val = max;

namespace Config
{
	using Direct3D9::Color;

	struct AimbotList;

	struct MainList
	{
		
		AimbotList* Aimbot = nullptr;
	};

	struct CurrentList
	{
		AimbotList* Aimbot = nullptr;
	};

	struct AimbotList
	{
		bool Resolver = false;
		bool Enabled = false;
		bool HitscanBone[7] = { false ,false,false,false,false,false,false };
		bool AutoStop = false;
		bool AutoScope = false;
		float hitchance = 1.0f;
		int target_idx = 0;
		Vector3	spotl{ 0,0,0 };
		int	MinDamage = 0;
		float	Minx = 0;
		float	Minz = 0;
		float	Miny = 0;
		bool PSilent = false;
		bool AimSpotOff = false;
		bool NoSpreadActive = false;
		bool MinDamageOverride = false;
		void Clamp()
		{
			LimitValue(MinDamage, 0, 150);
		}
	};


	struct ESPList
	{
		bool Enabled = false;
		bool Hitbox = false;
		int viewfov = 0;
		bool	Name = false;	
		bool	Weapon = false;		
		bool	AimSpot = false;
		bool		Health = false;
		bool		Armor = false;	
		int		Target = 0;
	};

	struct RenderList
	{
		bool ChamsOutlined[4]{ false,false,false,false };
		int ChamsMode[4]{ 0,0,0,0 };
		int	ChamsTarget = 0;
	};

	struct AntiAimList
	{
		bool FakeDuck = false;
		bool CustomAngles = false;
		int CustomAnglesR = 0;
		int CustomAnglesF = 0;
		bool NoEnemyEnabled = false;
		bool AtTargetEnabled = false;
		bool Slowwalk = false;
		bool slidewalk = false;
		bool Enabled = false;
		bool EnabledEx = false;
		bool Manual = false;
		bool FakeManuals = false;
		int FakeDuckKey = 0;
		int choked_commands = 0;
		int NoEnemy = 0;
		int side = 1;
		int manualside = 0;
		bool Adaptive = false;
		bool ShouldFakeDuck = false;
		bool EnableManuals = false;
		int flipkey = 0x70;
		int leftkey = VK_LEFT;
		int rightkey = VK_RIGHT;
		int backwardkey = VK_DOWN;
		int SlowwalkSpeed = 40;
		int SlowwalkKey = VK_SHIFT;

		void Clamp()
		{
			LimitValue(SlowwalkSpeed, 45, 85);
			LimitValue(CustomAnglesF, -180, 0);
			LimitValue(CustomAnglesR, 0, 180);
		}
	};

	struct RemovalsList
	{
		int		NoSpread = 0;			// 0 - Off | 1 - Normal | 2 - Advanced
		bool	SeedHelp = false;		// 
		bool	NoRecoil = false;		// 
		bool	NoFlash = false;		// 
		bool	NoVisualRecoil = false;	// 
		bool	NoSmoke = false;		// 

	};

	struct MiscList
	{
		int jjso = 72;
		int fr = 0;
		int ThirdPersonKey = 0;
		float ResolverAng = 0.0f;
		int Angles = 0;
		int target = 0;

		bool HitmarkerHP = false;
		bool aaa = false;
		bool HitmarkerEnabled = false;
		int Hitmarker = 3;
		float hitmarkerAlpha = 0.0f;
		

		int test0 = 5;

		int test = 5;


		int Val0 = 0;
		int Val1 = 0;
		int Val2 = 0;
		bool ThirdPerson = false;
		bool	AutoJump = false;	
		bool	FastDuck = false;
		bool	AutoPistol = false;		
		bool	AutoStrafe = false;		
		bool	BombWarning = false;	
		float ThirdXAdd = 0;
		float ThirdYAdd = 0;
		float ThirdZAdd = 0;
		float SpeedMod = 0.0f;
		bool	FakeLag = false;	

		int		ChokedPackets = 1;	
		void Clamp()
		{
			LimitValue(ChokedPackets, 1, 14);
			
			LimitValue(ThirdZAdd, 1.f, 200.f);
			LimitValue(ThirdXAdd, -180.f, 180.f);
			LimitValue(ThirdYAdd, -180.f, 180.f);
		}
	};

	struct ColorsList
	{
		int curr = 0;
		
		Color T_ESP_Normal = Color(255, 0, 0, 255);		// 
		Color T_ESP_Colored = Color(255, 255, 0, 255);	// 

		Color CT_ESP_Normal = Color(0, 128, 255, 255);	// 
		Color CT_ESP_Colored = Color(0, 255, 0, 255);		// 

		Color LOCAL_Chams = Color(255, 0, 0, 255);		// 
		Color LOCAL_Chams_Outline = Color(255, 0, 0, 255);		// 

		Color WEAPON_Chams_Normal = Color(255, 0, 0, 255);		// 
		Color WEAPON_Chams_Colored = Color(255, 0, 0, 255);		// 
		Color WEAPON_Chams_Outline = Color(255, 0, 0, 255);		// 
		float TestColor[3];
		Color ENEMY_Chams_Normal = Color(255, 0, 0, 255);		// 
		Color ENEMY_Chams_Colored = Color(255, 255, 0, 255);	// 
		Color ENEMY_Chams_Outline = Color(255, 255, 0, 255);	// 

		Color TEAM_Chams_Normal = Color(0, 128, 255, 255);	// 
		Color TEAM_Chams_Colored = Color(0, 255, 0, 255);	// 
		Color TEAM_Chams_Outline = Color(0, 255, 0, 255);	// 

		Color Main = Color(0, 0, 0, 255);				// main menu color
		Color Main2 = Color(0, 0, 0, 255);				// main2 menu color	
		Color Main3 = Color(0, 0, 0, 255);				// main3 menu color	
		Color Main4 = Color(0, 0, 0, 255);				// bg3 menu color
	};

	struct BindsList
	{
		int	Menu = 45;		// VK_INSERT
		int Eject = 122;		// VK_F11
		int Panic = 123;		// VK_F12
	};

	extern MainList* Main;
	extern CurrentList* Current;
	extern CurrentList* WeaponType[7];

	extern ESPList* ESP;
	extern RenderList* Render;
	extern AntiAimList* AntiAim;
	extern RemovalsList* Removals;
	extern MiscList* Misc;
	extern ColorsList* Colors;
	extern BindsList* Binds;

	extern void				Startup(HMODULE hMod);
	extern void				Release();

	extern void				Load(const std::string& name);
	extern void				Save(const std::string& name);
	extern void				Delete(const std::string& name);

	extern auto				GetPath()->std::string;
}