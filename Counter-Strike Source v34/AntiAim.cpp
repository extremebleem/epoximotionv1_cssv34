#include "AntiAim.h"
#include "Config.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Entity.hpp"

namespace Feature
{
	void AntiAim::AtTarget(CUserCmd* cmd, C_CSPlayer* player)
	{
		auto best = -1;
		auto distance = 8192.f;
		Vector3 end, aim;

		for (int i = 1; i <= Source::m_pEngine->GetMaxClients(); i++)
		{
			auto enemy = ToCSPlayer(Source::m_pEntList->GetBaseEntity(i));

			if (!enemy)
				continue;

			if (enemy == player)
				continue;

			if (enemy->m_iLifeState() != LIFE_ALIVE)
				continue;

			if (enemy->IsDormant())
				continue;

			if (player->m_iTeamNum() == enemy->m_iTeamNum())
				continue;

			if (!enemy->GetHitboxVector(12, end))
				continue;


			auto current = end.DistTo(player->EyePosition());

			if (current < distance)
			{
				distance = current;
				best = i;
			}
		}

		if (best != -1)
		{
			auto direction = end - player->EyePosition();

			VectorNormalize(direction);
			VectorAngles(direction, aim);

			cmd->viewangles.y = aim.y;
		}
	}

	void AntiAim::DoAntiAim(CUserCmd* cmd, C_CSPlayer* player, C_WeaponCSBaseGun* weapon, bool& bSendPacket)
	{
		C_BaseEntity* LocalPlayer = Source::m_pEntList->GetBaseEntity(Source::m_pEngine->GetLocalPlayer());

		if (cmd->buttons & IN_USE)return;

		if (player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP)return;

		if (weapon->GetWeaponType() == WEAPONTYPE_GRENADE)
		{
			if (weapon->m_fThrowTime() > 0.0f)
				return;
		}
		else if (weapon->GetWeaponType() == WEAPON_KNIFE && (((cmd->buttons & IN_ATTACK2) || (cmd->buttons & IN_ATTACK)) && weapon->m_flNextPrimaryAttack() < Source::m_pGlobalVars->curtime))
		{
			return;
		}
		else if (((cmd->buttons & IN_ATTACK2) || (cmd->buttons & IN_ATTACK)) && weapon->m_flNextPrimaryAttack() < Source::m_pGlobalVars->curtime)
		{
			return;
		}


		if (Config::AntiAim->AtTargetEnabled && !Config::AntiAim->Manual)
			AtTarget(cmd, player);

		if (GetAsyncKeyState(Config::AntiAim->flipkey) & 1)
			Config::AntiAim->side *= -1;
		if (Config::AntiAim->EnableManuals)
		{
			if (GetAsyncKeyState(Config::AntiAim->leftkey) & 1)
			{
				if (Config::AntiAim->manualside == 1 || Config::AntiAim->manualside == 0)
				{
					Config::AntiAim->Manual = !Config::AntiAim->Manual;
					if (Config::AntiAim->manualside == 1)
						Config::AntiAim->manualside = 0;
				}
				Config::AntiAim->manualside = 1;

			}

			if (GetAsyncKeyState(Config::AntiAim->rightkey))
			{
				if (Config::AntiAim->manualside == 2 || Config::AntiAim->manualside == 0)
				{
					Config::AntiAim->Manual = !Config::AntiAim->Manual;
					if (Config::AntiAim->manualside == 2)
						Config::AntiAim->manualside = 0;
				}
				Config::AntiAim->manualside = 2;

			}

			if (GetAsyncKeyState(Config::AntiAim->backwardkey))
			{
				if (Config::AntiAim->manualside == 3 || Config::AntiAim->manualside == 0)
				{
					Config::AntiAim->Manual = !Config::AntiAim->Manual;
					if (Config::AntiAim->manualside == 3)
						Config::AntiAim->manualside = 0;
				}
				Config::AntiAim->manualside = 3;
			}

			if (Config::AntiAim->manualside == 1 && Config::AntiAim->Manual)
			{
				cmd->viewangles.x = 89;
				if (Config::AntiAim->FakeManuals)
				{
					if (Config::AntiAim->choked_commands == 0)
						cmd->viewangles.y += 91;
					else
						cmd->viewangles.y -= 91;
				}
				else
					cmd->viewangles.y += 91;
			}
			else if (Config::AntiAim->manualside == 2 && Config::AntiAim->Manual)
			{
				cmd->viewangles.x = 89;
				if (Config::AntiAim->FakeManuals)
				{
					if (Config::AntiAim->choked_commands == 0)
						cmd->viewangles.y -= 91;
					else
						cmd->viewangles.y += 91;
				}
				else
					cmd->viewangles.y -= 91;
			}
			else if (Config::AntiAim->manualside == 3 && Config::AntiAim->Manual)
			{
				cmd->viewangles.x = 89;
				cmd->viewangles.y += 180;
			}
		}
		else
		{
			Config::AntiAim->manualside = 0;
			Config::AntiAim->Manual = false;
		}

		if (!Config::AntiAim->Manual && !Config::AntiAim->CustomAngles)
		{
			static float time = 0.0f;
			static bool jitter = false;
			cmd->viewangles.x = 87.662721f;
			if (!bSendPacket)
			{
				if (Config::AntiAim->EnabledEx)
				{
					if (!jitter)
						cmd->viewangles.y += 170;
					else
						cmd->viewangles.y += 300;
					if (Config::AntiAim->EnabledEx)
					{
						if (time + Config::Misc->FakeLag < Source::m_pGlobalVars->curtime)
						{
							cmd->viewangles.y += 359;
							time = Source::m_pGlobalVars->curtime;
						}
					}
				}
				else
				{
					if (!jitter)
						cmd->viewangles.y += 40 * Config::AntiAim->side;
					else
						cmd->viewangles.y -= 140 * Config::AntiAim->side;


					if (time + 11.0f < Source::m_pGlobalVars->curtime)
					{
						cmd->viewangles.y += 150;
						
						time = Source::m_pGlobalVars->curtime;
					}
				}
			}
			else
			{
				cmd->viewangles.y += 180;
				
			}

			jitter = !jitter;

		}
		else if (Config::AntiAim->CustomAngles && !Config::AntiAim->Manual)
		{
			cmd->viewangles.x = 87.662721f;
			if (bSendPacket)
			{
				
					cmd->viewangles.y += Config::AntiAim->CustomAnglesR * Config::AntiAim->side;
				
			}
			else
			{
				
					cmd->viewangles.y -= Config::AntiAim->CustomAnglesF * Config::AntiAim->side;
				
			}
		}
	}
	void AntiAim::FakeLag(CUserCmd* cmd, C_CSPlayer* Local,bool &bSendPacket)
	{
		int choke = Config::Misc->ChokedPackets;
		if (Config::AntiAim->Adaptive)
			choke = std::min< int >(static_cast<int>(std::ceilf(64 / (Local->m_vecVelocity().Length2D() * Source::m_pGlobalVars->interval_per_tick))), Config::Misc->ChokedPackets);
		if (choke > Config::AntiAim->choked_commands)
			bSendPacket = false;
	}
}