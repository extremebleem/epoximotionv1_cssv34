#include "Config.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Source.hpp"
#include "Menu.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

namespace Config
{
	MainList*		Main = nullptr;
	CurrentList*	Current = nullptr;
	CurrentList*	WeaponType[ 7 ];

	ESPList*		ESP = nullptr;
	RenderList*		Render = nullptr;
	AntiAimList*	AntiAim = nullptr;
	RemovalsList*	Removals = nullptr;
	MiscList*		Misc = nullptr;
	ColorsList*		Colors = nullptr;
	BindsList*		Binds = nullptr;

	std::string		m_config;
	std::string		m_current;

	void Startup(HMODULE hMod)
	{
		
			Main = new MainList();
			Main->Aimbot = new AimbotList();

			for (int i = 1; i < 7; i++)
			{
				WeaponType[i] = new CurrentList();

				WeaponType[i]->Aimbot = new AimbotList();
				
			}

			ESP = new ESPList();
			Render = new RenderList();
			AntiAim = new AntiAimList();
			Removals = new RemovalsList();
			Misc = new MiscList();
			Colors = new ColorsList();
			Binds = new BindsList();

			m_config = Shared::m_pVars->m_loader;
			m_config.append("\\CSSv34\\");

			Shared::m_strConfig = "default";
			Shared::m_bLoad = true;
			
		}
	

	void Release()
	{
		Memory::SafeDelete( Main->Aimbot );
		
		Memory::SafeDelete( Main );

		Memory::SafeDelete( Current->Aimbot );
		
		Memory::SafeDelete( Current );

		for( int i = 0; i < 7; i++ )
		{
			Memory::SafeDelete( WeaponType[ i ]->Aimbot );
			Memory::SafeDelete( WeaponType[ i ] );
		}
		Memory::SafeDelete( ESP );
		Memory::SafeDelete( Render );
		Memory::SafeDelete( AntiAim );
		Memory::SafeDelete( Removals );
		Memory::SafeDelete( Misc );
		Memory::SafeDelete( Colors );
		Memory::SafeDelete( Binds );
	}

	bool LoadBool( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );

		if( strcmp( szData, XorStr( "on" ) ) == 0 || strcmp( szData, XorStr( "true" ) ) == 0 || atoi( szData ) == 1 )
			return true;

		return false;
	}

	int LoadInt( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );
		return atoi( szData );
	}

	float LoadFloat( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );
		return ( float )atof( szData );
	}

	Color LoadColor( const std::string& strSection, const std::string& strName )
	{
		Color result;
		result.R = LoadInt( strSection, std::string( strName ).append( ".r" ) );
		result.G = LoadInt( strSection, std::string( strName ).append( ".g" ) );
		result.B = LoadInt( strSection, std::string( strName ).append( ".b" ) );
		result.A = LoadInt( strSection, std::string( strName ).append( ".a" ) );
		return result;
	}

	void SaveBool( const std::string& strSection, const std::string& strName, bool bValue )
	{
		const char* szData = bValue ? XorStr( "true" ) : XorStr( "false" );
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), szData, m_current.c_str() );
	}

	void SaveInt( const std::string& strSection, const std::string& strName, int iValue )
	{
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), std::to_string( iValue ).c_str(), m_current.c_str() );
	}

	void SaveFloat( const std::string& strSection, const std::string& strName, float flValue )
	{
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), std::to_string( flValue ).c_str(), m_current.c_str() );
	}

	void SaveColor( const std::string& strSection, const std::string& strName, const Color& color )
	{
		SaveInt( strSection, std::string( strName ).append( ".r" ), color.R );
		SaveInt( strSection, std::string( strName ).append( ".g" ), color.G );
		SaveInt( strSection, std::string( strName ).append( ".b" ), color.B );
		SaveInt( strSection, std::string( strName ).append( ".a" ), color.A );
	}

	bool Exists( const std::string& name )
	{
		std::ifstream f( name );
		return f.good();
	}

	bool Create( const std::string& name )
	{
		auto dir = name.substr( 0, name.find_last_of( "\\" ) );

		if( CreateDirectory( dir.c_str(), nullptr ) || GetLastError() == ERROR_ALREADY_EXISTS )
		{
			std::ofstream f( name );

			if( f.good() )
			{
				f.close();

				return true;
			}
		}

		return false;
	}

	void Load( const std::string& name )
	{
		auto current = name;

		if( current.find(".cfg") == std::string::npos )
			current.append(".cfg");

		m_current = m_config + current;

		if( !Exists( m_current ) )
			Save( name );

		std::string main("main");
		for (int i = 1; i < 7; i++)
		{
			std::ostringstream stream;
			stream << i;
			std::string temp = stream.str();
			WeaponType[i]->Aimbot->MinDamageOverride = LoadBool(temp, "aimbot.min.damage.override");
			WeaponType[i]->Aimbot->MinDamage = LoadInt(temp, "aimbot.min.damage");
		}

		Main->Aimbot->Enabled				= LoadBool( main, XorStr( "aimbot.enabled" ) );
		Main->Aimbot->AutoStop			= LoadBool( main, XorStr( "aimbot.auto.stop" ) );
		Main->Aimbot->Resolver			= LoadBool(main, XorStr("aimbot.resolver"));
		Main->Aimbot->MinDamage			= LoadInt( main, XorStr( "aimbot.min.damage" ) );
		Main->Aimbot->PSilent			= LoadBool( main, XorStr( "aimbot.psilent" ) );
		Main->Aimbot->NoSpreadActive	= LoadBool(main, XorStr("aimbot.no.spread.active"));
		Main->Aimbot->Clamp();
		ESP->Name						= LoadBool( main, XorStr( "esp.name" ) );
		ESP->Weapon						= LoadBool( main, XorStr( "esp.weapon" ) );
		ESP->AimSpot					= LoadBool( main, XorStr( "esp.aimspot" ) );
		ESP->Health						= LoadInt( main, XorStr( "esp.health" ) );
		ESP->Armor						= LoadInt( main, XorStr( "esp.armor" ) );
		ESP->Target						= LoadInt( main, XorStr( "esp.target" ) );
		for(int i = 1;i < 4;i++)Render->ChamsOutlined[i] = LoadBool(main, XorStr("render.chams.outlined"));
		
		Render->ChamsTarget				= LoadInt(main, XorStr("render.chams.target"));
		AntiAim->AtTargetEnabled = LoadBool(main, XorStr("antiaim.at.target.enabled"));
		AntiAim->NoEnemyEnabled = LoadBool(main, XorStr("antiaim.no.enemy.enabled"));
		AntiAim->NoEnemy = LoadInt(main, XorStr("antiaim.no.enemy"));
		AntiAim->Clamp();
		Removals->NoRecoil				= LoadBool( main, XorStr( "removals.no.recoil" ) );
		Removals->NoVisualRecoil		= LoadBool( main, XorStr( "removals.no.visual.recoil" ) );
		Removals->NoSmoke				= LoadBool( main, XorStr( "removals.no.smoke" ) );
		Misc->AutoJump = LoadBool(main, XorStr("misc.auto.jump"));
		Misc->ThirdPerson = LoadBool(main, XorStr("misc.thirdperson"));
		Misc->ThirdPersonKey = LoadInt(main, XorStr("misc.thirdperson.key"));
		Misc->Hitmarker = LoadInt(main, XorStr("misc.hitsound"));
		Misc->HitmarkerEnabled = LoadBool(main, XorStr("misc.hitmarker"));
		Misc->HitmarkerHP = LoadBool(main, XorStr("misc.hitmarker.hp"));
		Misc->AutoPistol				= LoadBool( main, XorStr( "misc.auto.pistol" ) );
		Misc->AutoStrafe				= LoadInt( main, XorStr( "misc.auto.strafe" ) );
		Misc->BombWarning				= LoadBool( main, XorStr( "misc.bomb.warning" ) );
	
		Misc->FakeLag					= LoadBool( main, XorStr( "misc.fakelag" ) );
		Misc->ChokedPackets				= LoadInt( main, XorStr( "misc.fakelag.choked.packets" ) );
		
		
		
		Misc->SpeedMod = LoadFloat(main, XorStr("misc.cstrafer.modifer"));
		
		Misc->ResolverAng = LoadFloat(main, XorStr("misc.resolver.angle"));

		Misc->Clamp();

		std::string colors( XorStr( "colors" ) );

		Colors->T_ESP_Normal			= LoadColor( colors, XorStr( "t.esp.normal" ) );
		Colors->T_ESP_Colored			= LoadColor( colors, XorStr( "t.esp.visible" ) );
		//Colors->T_Chams_Normal			= LoadColor( colors, XorStr( "t.chams.normal" ) );
		//Colors->T_Chams_Colored			= LoadColor( colors, XorStr( "t.chams.visible" ) );
		Colors->CT_ESP_Normal			= LoadColor( colors, XorStr( "ct.esp.normal" ) );
		Colors->CT_ESP_Colored			= LoadColor( colors, XorStr( "ct.esp.visible" ) );
		//Colors->CT_Chams_Normal			= LoadColor( colors, XorStr( "ct.chams.normal" ) );
		//Colors->CT_Chams_Colored = LoadColor(colors, XorStr("ct.chams.visible"));
	//	Colors->Crosshair = LoadColor(colors, XorStr("crosshair"));

		Source::m_pMenu->SetColors();
		Source::m_pMenu->ApplyColors();

		std::string binds( XorStr( "binds" ) );

		Binds->Menu						= LoadInt( binds, XorStr( "menu" ) );
		Binds->Eject					= LoadInt( binds, XorStr( "eject" ) );
		Binds->Panic					= LoadInt( binds, XorStr( "panic" ) );
	}

	void Save(const std::string& name)
	{
		auto current = name;

		if (current.find(XorStr(".cfg")) == std::string::npos)
			current.append(XorStr(".cfg"));

		m_current = m_config + current;

		if (!Exists(m_current))
		{
			if (!Create(m_current))
				DPRINT(XorStr("[Config::Save] Can't create config directory or file!"));
		}

		std::string main(XorStr("main"));

		Main->Aimbot->Clamp();

		for (int i = 1; i < 7; i++)
		{
			std::ostringstream stream;
			stream << i;
			std::string temp = stream.str();
			SaveBool(temp, "aimbot.min.damage.override", WeaponType[i]->Aimbot->MinDamageOverride);
			SaveInt(temp, "aimbot.min.damage", WeaponType[i]->Aimbot->MinDamage);
		}
		SaveInt(main, XorStr("aimbot.enabled"), Main->Aimbot->Enabled);
		SaveBool(main, XorStr("aimbot.auto.stop"), Main->Aimbot->AutoStop);
		SaveBool(main, XorStr("aimbot.resolver"), Main->Aimbot->Resolver);
		SaveInt(main, XorStr("aimbot.min.damage"), Main->Aimbot->MinDamage);
		SaveBool(main, XorStr("aimbot.psilent"), Main->Aimbot->PSilent);
		SaveBool(main, XorStr("aimbot.no.spread.active"), Main->Aimbot->NoSpreadActive);
		SaveBool(main, XorStr("esp.name"), ESP->Name);
		SaveBool(main, XorStr("esp.weapon"), ESP->Weapon);
		SaveBool(main, XorStr("esp.aimspot"), ESP->AimSpot);
		SaveInt(main, XorStr("esp.health"), ESP->Health);
		SaveInt(main, XorStr("esp.armor"), ESP->Armor);
		SaveInt(main, XorStr("esp.target"), ESP->Target);
		
		for(int i = 1;i<4;i++)
			SaveBool(main, XorStr("render.chams.outlined"), Render->ChamsOutlined[i]);
		SaveInt(main, XorStr("render.chams.target"), Render->ChamsTarget);
		AntiAim->Clamp();

		SaveBool(main, XorStr("antiaim.at.target.enabled"), AntiAim->AtTargetEnabled);


		SaveBool(main, XorStr("antiaim.no.enemy.enabled"), AntiAim->NoEnemyEnabled);
		SaveInt(main, XorStr("antiaim.no.enemy"), AntiAim->NoEnemy);

		SaveBool(main, XorStr("antiaim.Slowwalk"), AntiAim->Slowwalk);
		SaveInt(main, XorStr("antiaim.Slowwalk.key"), AntiAim->SlowwalkKey);


		
		SaveBool(main, XorStr("removals.no.recoil"), Removals->NoRecoil);
		SaveBool(main, XorStr("removals.no.visual.recoil"), Removals->NoVisualRecoil);
		SaveBool(main, XorStr("removals.no.smoke"), Removals->NoSmoke);
		

		Misc->Clamp();

		SaveBool(main, XorStr("misc.auto.jump"), Misc->AutoJump);
		SaveBool(main, XorStr("misc.thirdperson"), Misc->ThirdPerson);
		SaveInt(main, XorStr("misc.thirdperson.key"), Misc->ThirdPersonKey);
		SaveInt(main, XorStr("misc.hitsound"), Misc->Hitmarker);
		SaveBool(main, XorStr("misc.hitmarker"), Misc->HitmarkerEnabled);
		SaveBool(main, XorStr("misc.hitmarker.hp"), Misc->HitmarkerHP);
		SaveBool(main, XorStr("misc.auto.pistol"), Misc->AutoPistol);
		SaveInt(main, XorStr("misc.auto.strafe"), Misc->AutoStrafe);
		SaveBool(main, XorStr("misc.bomb.warning"), Misc->BombWarning);
	
		SaveBool(main, XorStr("misc.fakelag"), Misc->FakeLag);
		SaveInt(main, XorStr("misc.fakelag.choked.packets"), Misc->ChokedPackets);
		
		
		
		SaveFloat(main, XorStr("misc.cstrafer.modifer"), Misc->SpeedMod);

		SaveFloat(main, XorStr("misc.resolver.angle"), Misc->ResolverAng);


		std::string colors(XorStr("colors"));

		SaveColor(colors, XorStr("t.esp.normal"), Colors->T_ESP_Normal);
		SaveColor(colors, XorStr("t.esp.visible"), Colors->T_ESP_Colored);
		//SaveColor(colors, XorStr("t.chams.normal"), Colors->T_Chams_Normal);
		//SaveColor(colors, XorStr("t.chams.visible"), Colors->T_Chams_Colored);
		SaveColor(colors, XorStr("ct.esp.normal"), Colors->CT_ESP_Normal);
		SaveColor(colors, XorStr("ct.esp.visible"), Colors->CT_ESP_Colored);
		//SaveColor(colors, XorStr("ct.chams.normal"), Colors->CT_Chams_Normal);
		//SaveColor(colors, XorStr("ct.chams.visible"), Colors->CT_Chams_Colored);
		//SaveColor(colors, XorStr("crosshair"), Colors->Crosshair);


		std::string binds(XorStr("binds"));

		SaveInt(binds, XorStr("menu"), Binds->Menu);
		SaveInt(binds, XorStr("eject"), Binds->Eject);
		SaveInt(binds, XorStr("panic"), Binds->Panic);
	}

	void Delete( const std::string& name )
	{
		auto current = name;

		if( current.find( XorStr( ".cfg" ) ) == std::string::npos )
			current.append( XorStr( ".cfg" ) );

		m_current = m_config + current;

		DeleteFileA( m_current.c_str() );
	}

	auto GetPath() -> std::string
	{
		return m_config;
	}
}