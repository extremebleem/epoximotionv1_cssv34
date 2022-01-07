#pragma once
#include "Valve.hpp"

namespace Feature
{
	class AntiAim
	{
	public:
		QAngle FakeAngle;
		QAngle RealAngle;
		QAngle angl;
		void DoAntiAim(CUserCmd* cmd, C_CSPlayer* player, C_WeaponCSBaseGun* weapon, bool& bSendPacket);
		void AtTarget(CUserCmd* cmd, C_CSPlayer* player);
		void FakeLag(CUserCmd* cmd, C_CSPlayer* Local, bool& bSendPacket);
	};
}