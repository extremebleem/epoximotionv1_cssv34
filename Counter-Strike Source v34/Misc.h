#pragma once
#include "Valve.hpp"

namespace Feature
{
	class MiscClass
	{
	public:
		void RotateMovement(CUserCmd* cmd, float rotation);
		void Slowwalk(CUserCmd* cmd, C_CSPlayer* player);
		void FakeDuck(CUserCmd* cmd, bool& bSendPacket);
		void RemoveInterpolation();
		void ApplyStrafe(CUserCmd* cmd, const Vector3& va);
		void AutoStrafe(CUserCmd* cmd, C_CSPlayer* player);
		void BunnyHop(CUserCmd* cmd, C_CSPlayer* player);
		void Speed(C_CSPlayer* player, CUserCmd* cmd, Vector3& Originalview);

	};
}
