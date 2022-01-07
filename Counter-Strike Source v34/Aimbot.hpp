#pragma once

#include "Valve.hpp"

namespace Feature
{
	class Aimbot
	{
	public:
		void RageAim(CUserCmd * cmd, C_WeaponCSBaseGun * weapon,bool &bSendPacket);
		void NoSpread(CUserCmd* cmd, C_WeaponCSBaseGun* weapon);
		void AutoPistol(CUserCmd* cmd, C_WeaponCSBaseGun* weapon);
		void NoRecoil(CUserCmd* cmd, C_CSPlayer* player, C_WeaponCSBaseGun* weapon);
		float oldWeaponTime = 0.0f;
	private:
		bool scoped = false;
		C_WeaponCSBaseGun* oldWeapon;
		int scope_count = 0;
	};
	
}