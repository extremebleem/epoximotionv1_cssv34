#include "Aimbot.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Source.hpp"
#include "Hooked.hpp"
#include "Player.hpp"
#include "Config.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "cdll_int.hpp"
#include "Core.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

#define M_PI ( float )3.14159265358979323846

bool IsEveryoneDead()
{
	auto local = C_CSPlayer::GetLocalPlayer();
	for (auto i = 0; i <= Source::m_pEngine->GetMaxClients(); i++)
	{
		auto player = C_CSPlayer::GetPlayer(i);
		if (player && player != local && player->m_iLifeState() == LIFE_ALIVE)
		{
			if (player->m_iTeamNum() == local->m_iTeamNum())
				continue;
			return false;
		}
	}
	return true;
}

float ScaleDamage(float damage, int armor)
{
	float ratio = 0.5f;
	float bonus = 0.5f;

	if (armor > 0)
	{
		float new_damage = damage * ratio;
		float new_armor = (damage - new_damage) * bonus;

		if (new_armor > static_cast<float>(armor))
		{
			new_armor = static_cast<float>(armor) * (1.0f / bonus);
			new_damage = damage - new_armor;
		}

		damage = new_damage;
	}

	return damage;
}

int SelectAimPoint(std::vector<int> hitbox, C_WeaponCSBaseGun* Weapon, C_CSPlayer* Entity)
{
	float multipler = 1.0f;
	float damage = Weapon->GetCSWpnData()->m_iDamage;
	Vector3 vTemp;
	std::vector<float> damages;

	for (int i : hitbox)
	{
		if (i == 12 || i == 11)multipler = 4.0f;
		else if (i == 0 || i == 9 || i == 10)multipler = 1.25f;
		else if (i > 0 && i < 9)multipler = 0.75;
		else multipler = 1.0f;
		damage *= multipler;
		damage = ScaleDamage(damage, Entity->m_ArmorValue());
		damages.push_back(damage);
	}
	return std::max_element(damages.begin(), damages.end()) - damages.begin();
}

C_BaseEntity* ChangeTarget(std::vector<float> Distances)
{
	std::vector<float> sortedDistances = Distances;
	std::sort(sortedDistances.begin(), sortedDistances.end());

	int nextMinDistIdx = (std::min_element(sortedDistances.begin(), sortedDistances.end()) - sortedDistances.begin()) + 1;
	int nextMinDistOrigIdx = std::find(Distances.begin(), Distances.end(), sortedDistances[nextMinDistIdx]) - Distances.begin();

	return Source::m_pEntList->GetBaseEntity(nextMinDistOrigIdx);
}

namespace Feature
{
	void Aimbot::AutoPistol(CUserCmd* cmd, C_WeaponCSBaseGun* weapon)
	{
		if (Source::m_pGlobalVars->curtime < weapon->m_flNextPrimaryAttack() && weapon->GetWeaponType() == WEAPONTYPE_PISTOL)
				cmd->buttons &= ~IN_ATTACK;
	}

	std::vector<float> GetDistances(C_CSPlayer* LocalPlayer, std::vector<int> players)
	{
		std::vector<float> allDistances;
		
		for (int i : players)
		{
			Vector3 tempo;
			float dst = -1;
			C_BaseEntity* entityObj = Source::m_pEntList->GetBaseEntity(i);

			if (ToCSPlayer(entityObj)->m_iLifeState() != LIFE_ALIVE)continue;
			if (!ToCSPlayer(entityObj)->GetHitboxVector(0, tempo))continue;

			dst = LocalPlayer->EyePosition().DistTo(tempo);

			allDistances.push_back(dst);
		}

		return allDistances;
	}

	void Aimbot::NoSpread(CUserCmd* cmd, C_WeaponCSBaseGun* weapon)
	{
		if (cmd->buttons & IN_ATTACK && weapon->m_flNextPrimaryAttack() < Source::m_pGlobalVars->curtime)
		{
			cmd->random_seed = 141;
			while ((MD5_PseudoRandom(cmd->command_number) & 255) != cmd->random_seed)
				cmd->command_number++;
			Source::m_pAccuracy->ApplySpreadFix(weapon, cmd->random_seed, cmd->viewangles, cmd->viewangles);
		}
	}

	void Aimbot::NoRecoil(CUserCmd* cmd, C_CSPlayer* player, C_WeaponCSBaseGun* weapon)
	{
		cmd->viewangles -= player->m_vecPunchAngle() * 2.0f;
	}

	const char* oldWeaponName = "";
	void Aimbot::RageAim(CUserCmd* cmd, C_WeaponCSBaseGun* weapon, bool& bSendPacket)
	{
		// DEFINES
		std::vector<int> playerIndexes;
		std::vector<int> penetratablePlayers;
		std::vector<int> penetrarableHitboxes;
		std::vector<int> HitboxesToScan;
		std::vector<Vector3> penetrarableHitboxesPos;
		std::vector<Vector3> visibleHitboxesPos;
		int entityIdx = -1;
		Vector3 vMins, vMaxs, vTemp;
		C_BaseEntity* Entity_Obj;

		// INIT
		C_BaseEntity* LocalPlayer_obj = Source::m_pEntList->GetBaseEntity(Source::m_pEngine->GetLocalPlayer());
		C_CSPlayer* LocalPlayer = ToCSPlayer(LocalPlayer_obj);	if (!LocalPlayer || LocalPlayer->m_iLifeState() != LIFE_ALIVE)return;
		C_WeaponCSBaseGun* Weapon = LocalPlayer->GetActiveWeapon();	if (!Weapon)return;

		if (IsEveryoneDead())return;

		if (Weapon->GetName() != oldWeaponName)
		{
			oldWeaponName = Weapon->GetName();
			oldWeaponTime = Source::m_pGlobalVars->curtime;
		}

		if (oldWeaponTime + 1 > Source::m_pGlobalVars->curtime)
			return;

		if (Weapon->IsMelee())return;
		// CHECK FOR PLAYER
		for (int i = 0; i <= Source::m_pEntList->GetHighestEntityIndex(); i++)
		{
			C_BaseEntity* Entity_obj = Source::m_pEntList->GetBaseEntity(i);

			if (!Entity_obj || Entity_obj == LocalPlayer_obj || !Entity_obj->IsPlayer() || Entity_obj->m_iTeamNum() == LocalPlayer_obj->m_iTeamNum())continue;
			if (ToCSPlayer(Entity_obj)->m_iLifeState() == LIFE_ALIVE)
				playerIndexes.push_back(i);
		}

		if (playerIndexes.empty())return;

		/*for (int x = 0; x <= 18; x++)
		{
			if ((Config::Main->Aimbot->HitscanBone[1] && x == 12) ||
				(Config::Main->Aimbot->HitscanBone[3] && x == 0) ||
				(Config::Main->Aimbot->HitscanBone[4] && x == 9) ||
				(Config::Main->Aimbot->HitscanBone[5] && x == 10) ||
				(Config::Main->Aimbot->HitscanBone[6] && x >= 13) ||
				(Config::Main->Aimbot->HitscanBone[7] && x > 0 && x < 9))
			{
				HitboxesToScan.push_back(x);
			}
		}*/

		//if(HitboxesToScan.empty())return;

		std::vector<float> Distances = GetDistances(LocalPlayer, playerIndexes);

		int iScanned = 0;

		int minDistanceIdx = std::min_element(Distances.begin(), Distances.end()) - Distances.begin();
		float minDistance = *std::min_element(Distances.begin(), Distances.end());

		Entity_Obj = Source::m_pEntList->GetBaseEntity(playerIndexes[minDistanceIdx]);

		C_CSPlayer* entity = ToCSPlayer(Entity_Obj);

		static bool Scanned = true;

		if (!Scanned)
		{
			Entity_Obj = ChangeTarget(Distances);
		}

		entity->GetHitboxVector(12, vMins);
		Scanned = true;
		if (!Source::m_pAccuracy->CanPenetrate(LocalPlayer->EyePosition(), vMins, Config::Main->Aimbot->MinDamage))
		{
			entity->GetHitboxBounds(12, vMins, vMaxs);
			vMins.z += 1.f;
			vMins.y -= 1.f;
			vMins.x -= 1.f;
			if (!Source::m_pAccuracy->CanPenetrate(LocalPlayer->EyePosition(), vMins, Config::Main->Aimbot->MinDamage))
			{
				Scanned = false;
			}
		}

		if (Scanned == true)
		{
			Config::Main->Aimbot->spotl = vMins;
			vMins -= LocalPlayer->EyePosition();
			VectorNormalize(vMins);
			VectorAngles(vMins, vMaxs);
			vMaxs.z = 0;
			AnglesNormalize(vMaxs);

			if (!weapon->m_iClip1())
				cmd->buttons |= IN_RELOAD;
			else
			{
				cmd->buttons |= IN_ATTACK;
				if (Config::Main->Aimbot->PSilent)
					bSendPacket = false;
				VectorCopy(vMaxs, cmd->viewangles);
			}
		}
	}
}
