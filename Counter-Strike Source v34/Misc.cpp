#include "Misc.h"
#include "Config.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Entity.hpp"

namespace Feature
{
	void MiscClass::RotateMovement(CUserCmd* cmd, float rotation)
	{
		rotation = ToRadians(rotation);
		float cosr, sinr;
		cosr = cos(rotation);
		sinr = sin(rotation);
		float forwardmove, sidemove;
		forwardmove = (cosr * cmd->forwardmove) - (sinr * cmd->sidemove);
		sidemove = (sinr * cmd->forwardmove) - (cosr * cmd->sidemove);
		cmd->forwardmove = forwardmove;
		cmd->sidemove = sidemove;
	}
	void MiscClass::Slowwalk(CUserCmd* cmd, C_CSPlayer* player)
	{
		if (player->GetLocalPlayer()->GetFlags() & FL_ONGROUND)
		{
			int speed = player->m_vecVelocity().Length2D();
			if (GetAsyncKeyState(Config::AntiAim->SlowwalkKey))
			{
				if (speed < Config::AntiAim->SlowwalkSpeed)return;
				if (GetAsyncKeyState(83))
					cmd->forwardmove = Config::AntiAim->SlowwalkSpeed * -1;
				if (GetAsyncKeyState(87))
					cmd->forwardmove = Config::AntiAim->SlowwalkSpeed;
				if (GetAsyncKeyState(68))
					cmd->sidemove = Config::AntiAim->SlowwalkSpeed;
				if (GetAsyncKeyState(65))
					cmd->sidemove = Config::AntiAim->SlowwalkSpeed * -1;
			}
		}
	}
	void MiscClass::FakeDuck(CUserCmd* cmd, bool& bSendPacket)
	{
		if (GetAsyncKeyState(Config::AntiAim->FakeDuckKey) & 1)
		{
			Config::AntiAim->ShouldFakeDuck = true;
			static float time;
			if (Config::AntiAim->choked_commands < 15 && Source::m_pGlobalVars->curtime > time)
			{
				cmd->buttons &= ~IN_DUCK;
				bSendPacket = false;
				time = Source::m_pGlobalVars->curtime + 0.5f;
			}
			else
			{
				cmd->buttons |= IN_DUCK;
			}
		}
		else
			Config::AntiAim->ShouldFakeDuck = false;
	}
	void MiscClass::RemoveInterpolation()
	{
		static std::uintptr_t interpolation_list = (std::uintptr_t)ModuleD(XorStr("client.dll")) + 0x390028;

		*(std::uint16_t*)(interpolation_list + 12) = 0xFFFF;
		*(std::uint16_t*)(interpolation_list + 18) = 0;
	}

	void MiscClass::ApplyStrafe(CUserCmd* cmd, const Vector3& va)
	{

		float yaw, speed;

		Vector3& move = *(Vector3*)&cmd->forwardmove;

		speed = move.Length2D();

		yaw = ToDegrees(atan2(move.y, move.x));
		yaw = ToRadians(cmd->viewangles.y - va.y + yaw);

		move.x = cos(yaw) * speed;
	}
	void MiscClass::AutoStrafe(CUserCmd* cmd, C_CSPlayer* player)
	{
		if (player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP)
			return;

		if (player->m_iLifeState() == LIFE_DEAD)
			return;

		if (player->GetLocalPlayer()->GetFlags() & FL_ONGROUND)
			return;

		float speed = player->m_vecVelocity().Length2D();

		float tickrate = 1.0f;

		if ((Source::m_pGlobalVars->interval_per_tick * 100) > 1.0f)
			tickrate = 1.1f;

		static float yaw;

		yaw = AngleNormalize(cmd->viewangles.y - yaw);

		Vector3 strafe = cmd->viewangles;

		if (Config::Misc->AutoStrafe == 1) // Normal
		{
			if (yaw < 0.0f)
				cmd->sidemove = 400.0f;
			else if (yaw > 0.0f)
				cmd->sidemove = -400.0f;
		}
		
		float value = (8.15f - tickrate) - (speed / 340.0f);

		if (speed > 160.0f && speed < 420.0f)
			value = (4.6f - tickrate) - (speed / 340.0f);

		if (speed > 420.0f)
			value = (3.0f - tickrate) - (speed / 1000.0f);

		if (value <= 0.275f)
			value = 0.275f;

		if (abs(yaw) < value)
		{
			static bool direction = false;

			if (direction)
			{
				strafe.y -= value;
				cmd->sidemove = -400.0f;
			}
			else
			{
				strafe.y += value;
				cmd->sidemove = 400.0f;
			}

			direction = !direction;

			ApplyStrafe(cmd, strafe);
		}
		else
		{
			if (yaw < 0.0f)
				cmd->sidemove = 400.0f;
			else if (yaw > 0.0f)
				cmd->sidemove = -400.0f;
		}

		yaw = cmd->viewangles.y;
	}
	void MiscClass::BunnyHop(CUserCmd* cmd, C_CSPlayer* player) {
		if (player->GetLocalPlayer()->GetFlags() & FL_ONGROUND)
			cmd->buttons &= ~IN_JUMP;
	}
	void MiscClass::Speed(C_CSPlayer* player, CUserCmd* cmd, Vector3& Originalview)
	{

		static float yaw;
		if (!Shared::m_bSpeed)
		{
			yaw = 0;
			return;
		}

		if (Source::m_pDataManager->GetFlags() & FL_ONGROUND)
			return;
		yaw = AngleNormalize(cmd->viewangles.y - yaw);

		if (player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP)
			return;
		if (player->m_iLifeState() == LIFE_DEAD)
			return;
		if (GetAsyncKeyState(VK_SPACE))
		{
			Vector3 View(cmd->viewangles);
			float blackcock = 0;
			cmd->forwardmove = 450.f;
			int random = rand() % 100;
			int random2 = rand() % 1000;
			static bool dir;
			static float current_y = View.y;
			if (player->m_vecVelocity().Length() > 50.f)
			{
				blackcock += 0.00007;
				current_y += (Config::Misc->SpeedMod) - blackcock;
			}
			else
			{
				blackcock = 0;
			}
			View.y = current_y;
			if (random == random2)
				View.y += random;
			// Clamp(View);
			else
			{
				float blackcock = 0;
			}
			RotateMovement(cmd, current_y);
		}
	}
}