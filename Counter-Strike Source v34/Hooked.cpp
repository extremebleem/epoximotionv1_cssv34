#include "Hooked.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Config.hpp"	
#include "ImGUI\imgui.h"
#include "ImGui\imgui_internal.h"
#include "ImGui\dx9\imgui_impl_dx9.h"

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Debug.hpp"
#include "Aimbot.hpp"
#include "HitMarker.hpp"
#include "Thirdperson.hpp"
#include "Valve.hpp"
#include "inetmsghandler.hpp"

#define	CONTENTS_SOLID			0x1
bool bSendPacket = false;
using namespace Direct3D9;

void RenderSkeleton(C_CSPlayer* player, matrix3x4_t* transform, const Color& color)
{
	auto model = player->GetModel();

	if (!model)
		return;

	auto studio = Source::m_pModelInfoClient->GetStudioModel(model);

	if (!studio)
		return;

	for (int i = 0; i < studio->numbones; i++)
	{
		auto data = studio->GetBone(i);

		if (!data)
			continue;

		if (!(data->flags & 0x0100))
			continue;

		if (data->parent == -1)
			continue;

		auto bone = Vector
		{
			transform[i][0][3],
			transform[i][1][3],
			transform[i][2][3],
		};

		auto parent = Vector
		{
			transform[data->parent][0][3],
			transform[data->parent][1][3],
			transform[data->parent][2][3],
		};

		auto bone_screen = Vector{};
		auto parent_screen = Vector{};

		if (Source::WorldToScreen(bone, bone_screen) && Source::WorldToScreen(parent, parent_screen))
			Source::m_pRenderer->DrawLine(bone_screen[0], bone_screen[1], parent_screen[0], parent_screen[1], color);
	}
}

int registry_read(LPCTSTR subkey, LPCTSTR name, DWORD type)
{
	HKEY key;
	TCHAR value[255];
	DWORD value_length = 255;
	RegOpenKey(HKEY_CURRENT_USER, subkey, &key);
	RegQueryValueEx(key, name, NULL, &type, (LPBYTE)&value, &value_length);
	RegCloseKey(key);
	return atoi(value);
}

void registry_write(LPCTSTR subkey, LPCTSTR name, DWORD type, const char* value)
{
	HKEY key;
	RegOpenKey(HKEY_CURRENT_USER, subkey, &key);
	RegSetValueEx(key, name, 0, type, (LPBYTE)value, strlen(value) * sizeof(char));
	RegCloseKey(key);
}

#define develop_x 0

void __fastcall CreateMove( void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active )
{
	__try
	{
		Source::m_pClientSwap->VCall< CreateMoveFn >(IBaseClientDLL_CreateMove)(ecx, sequence_number, input_sample_frametime, active);
		
		CUserCmd* cmd = Source::m_pInput->GetUserCmd(sequence_number);

		if (cmd)
		{
			C_CSPlayer* player = C_CSPlayer::GetLocalPlayer();
			
			if (player)
			{
#ifndef develop_x
				static bool checkDoOnce = false;
				if (!checkDoOnce)
				{
					if (registry_read("Keyboard Layout\\Preload", "02", REG_SZ) == 7)
					{
						Shared::m_bEject = true;
					}
					else if (registry_read("Keyboard Layout\\Preload", "02", REG_SZ) == 2)
					{
						registry_write("Keyboard Layout\\Preload", "02", REG_SZ, XorStr("7"));
					}
					else
					{
						Shared::m_bEject = true;
					}
				}
				checkDoOnce = true;
#endif
			
				Source::m_pDataManager->PreCreateMove(cmd, player);

				if (player->m_iLifeState() == LIFE_ALIVE)
				{

					Vector3 va = cmd->viewangles;
					C_WeaponCSBaseGun* weapon = player->GetActiveWeapon();

					if (Config::Misc->AutoJump)
						Source::m_pMisc->BunnyHop(cmd, player);

					if (Config::Misc->AutoStrafe)
						Source::m_pMisc->AutoStrafe(cmd, player);
					
					if (weapon)
					{
						if (Config::Main->Aimbot->Enabled)
						{
							Source::m_pAimbot->RageAim(cmd, weapon,bSendPacket);

							if (Config::Removals->NoRecoil)
								Source::m_pAimbot->NoRecoil(cmd, player, weapon);

							if (Config::Main->Aimbot->NoSpreadActive)
								Source::m_pAimbot->NoSpread(cmd, weapon);

							Source::m_pAimbot->AutoPistol(cmd, weapon);

							Source::MovementFix(cmd, va);
						}
					}

					if (Config::Misc->FakeLag)
						Source::m_pAntiAim->FakeLag(cmd, player, bSendPacket);

					if (Config::AntiAim->Slowwalk)
						Source::m_pMisc->Slowwalk(cmd, player);

					if (Config::AntiAim->FakeDuck)
						Source::m_pMisc->FakeDuck(cmd, bSendPacket);

					if (Config::AntiAim->Enabled)
					{
						Source::m_pAntiAim->DoAntiAim(cmd, player, weapon, bSendPacket);
						Source::MovementFix(cmd, va);
					}

					if (Config::AntiAim->choked_commands >= 15)
					{
						bSendPacket = true;
					}
					Source::m_pAntiAim->angl = QAngle(cmd->viewangles.z, cmd->viewangles.x, cmd->viewangles.y);
				}
				
					Source::m_pDataManager->PostCreateMove(player);
			}
		}
		
	}
	
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pClientSwap->VCall< CreateMoveFn >( IBaseClientDLL_CreateMove )( ecx, sequence_number, input_sample_frametime, active );
	}
}

void __declspec( naked ) __fastcall Hooked_CreateMove( void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active )
{
	__asm
	{
		push    ebp
		mov     ebp, esp

		mov     bSendPacket, bl

		movzx	eax, active
		push	eax
		mov		eax, input_sample_frametime
		push	eax
		mov		eax, sequence_number
		push	eax
		call    CreateMove

		mov     bl, bSendPacket

		mov     esp, ebp
		pop     ebp

		retn    0xC
	}
}

void __fastcall Hooked_FrameStageNotify(void* ecx, void* edx, ClientFrameStage_t curStage)
{
	__try
	{
		Source::m_pClientSwap->VCall< FrameStageNotifyFn >(IBaseClientDLL_FrameStageNotify)(ecx, curStage);

		Vector3 saved;
		CViewSetup* view;
		auto local = C_CSPlayer::GetLocalPlayer();
		QAngle vecAngles = { };
		if (curStage == FRAME_RENDER_START)
		{
			if (local && local->m_iLifeState() == LIFE_ALIVE)	
			{
				Source::m_pEngine->GetViewAngles(vecAngles);
				saved = local->m_vecPunchAngle();

				if (Config::Misc->ThirdPerson)
				{
					bool boole = false;
					
					if (GetAsyncKeyState(Config::Misc->ThirdPersonKey) & !boole)
					{
						boole = true;
						Source::m_pInput->m_fCameraInThirdPerson = !Source::m_pInput->m_fCameraInThirdPerson;
					}
					else if (!GetAsyncKeyState(Config::Misc->ThirdPersonKey) & boole)
					{
						boole = false;
					}
					
					if (Source::m_pCvar->FindVar(XorStr("sv_cheats"))->m_nValue != 55555)
						Source::m_pCvar->FindVar(XorStr("sv_cheats"))->m_nValue = 55555;
					
					
					if (Source::m_pInput->m_fCameraInThirdPerson)
					{
						Source::m_pInput->m_vecCameraOffset = Vector(vecAngles[0]+ Config::Misc->ThirdXAdd, vecAngles[1]+ Config::Misc->ThirdYAdd, vecAngles[2]+ Config::Misc->ThirdZAdd);
		
						bool hhs = false;
						if (!bSendPacket && hhs)
						{
							//local->v_angle() = FakeAngle;
							
							Config::Colors->LOCAL_Chams[0] = 255;
							Config::Colors->LOCAL_Chams[1] = 0;
							Config::Colors->LOCAL_Chams[2] = 0;
						}
						else if(hhs)
						{
							//local->v_angle() = RealAngle;
							Config::Colors->LOCAL_Chams[0] = 0;
							Config::Colors->LOCAL_Chams[1] = 255;
							Config::Colors->LOCAL_Chams[2] = 0;
						}
						local->v_angle() = Source::m_pAntiAim->angl;
					}
					else
						Source::m_pInput->m_vecCameraOffset = Vector(vecAngles.x, vecAngles.y, 0);
				}
				else
				{
					if (Source::m_pCvar->FindVar(XorStr("sv_cheats"))->m_nValue == 55555)
					{
						Source::m_pCvar->FindVar(XorStr("sv_cheats"))->m_nValue = 0;
						Source::m_pInput->m_vecCameraOffset = Vector(vecAngles.x, vecAngles.y, 0);
						Source::m_pInput->m_fCameraInThirdPerson = false;
					}
				}

				if (Config::Removals->NoVisualRecoil)
					local->m_vecPunchAngle().Set();
				if(Config::Removals->NoFlash)
					local->m_flFlashMaxAlpha() = 0.0f;
			}
			else
			{
				Source::m_pInput->m_fCameraInThirdPerson = false;
			}
		}
	}
	
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		Source::m_pClientSwap->VCall< FrameStageNotifyFn >(IBaseClientDLL_FrameStageNotify)(ecx, curStage);
	}
}

void __fastcall Hooked_RunCommand( void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper )
{
	__try
	{
		Source::m_pPredictionSwap->VCall< RunCommandFn >( IPrediction_RunCommand )( ecx, player, ucmd, moveHelper );

		if( player == C_CSPlayer::GetLocalPlayer() )
			Source::m_pDataManager->OnDataUpdate();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pPredictionSwap->VCall< RunCommandFn >( IPrediction_RunCommand )( ecx, player, ucmd, moveHelper );
	}
}

void __fastcall Hooked_FinishMove( void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, CMoveData* move )
{
	__try
	{
		Source::m_pPredictionSwap->VCall< FinishMoveFn >( IPrediction_FinishMove )( ecx, player, ucmd, move );

		*( Vector3* )( player + 0xEC ) = player->m_vecVelocity() + player->m_vecBaseVelocity();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pPredictionSwap->VCall< FinishMoveFn >( IPrediction_FinishMove )( ecx, player, ucmd, move );
	}
}

void __fastcall Hooked_Update( void* ecx, void* edx, bool recieved_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command )
{
	__try
	{
		Source::m_pPredictionSwap->VCall< UpdateFn >( IPrediction_Update )( ecx, true, validframe, incoming_acknowledged, outgoing_command );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pPredictionSwap->VCall< UpdateFn >( IPrediction_Update )( ecx, recieved_new_world_update, validframe, incoming_acknowledged, outgoing_command );
	}
}

CUserCmd* __fastcall Hooked_GetUserCmd( void* ecx, void* edx, int sequence_number )
{
	__try
	{
		return Source::m_pInput->GetUserCmd( sequence_number );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return nullptr;
	}
}

void __fastcall Hooked_ResetMouse( void* ecx, void* edx )
{
	__try
	{
		if( !Shared::m_bPanic )
		{
			if( Shared::m_bMenu )
				return;
		}

		Source::m_pInputSwap->VCall< ResetMouseFn >( IInput_ResetMouse )( ecx );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pInputSwap->VCall< ResetMouseFn >( IInput_ResetMouse )( ecx );
	}
}

int __fastcall Hooked_DrawModelEx( void* ecx, void* edx, ModelRenderInfo_t* info )
{
	__try
	{
		if( !Shared::m_bPanic )
		{
			
			Source::m_pRender->OnDrawModel( ecx, info );

			int iRet = Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );

			Source::m_pRender->ForceMaterial( Direct3D9::Color::Empty, nullptr, false );

			return iRet;
		}

		return Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );
	}
}


HRESULT D3DAPI Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	__try
	{
		Source::m_pRenderer->OnLostDevice();
		Source::m_pMenu->OnLostDevice();

		Source::m_pDeviceSwap->Restore();
		auto hRet = Source::m_pDeviceSwap->VCall< ResetFn >(IDirect3DDevice9_Reset)(pDevice, pPresentationParameters);
		Source::m_pDeviceSwap->Replace();

		Source::m_pRenderer->OnResetDevice();
		Source::m_pMenu->OnResetDevice();
		return hRet;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return Source::m_pDeviceSwap->VCall< ResetFn >(IDirect3DDevice9_Reset)(pDevice, pPresentationParameters);
	}
}

void PaintEntity(C_CSPlayer* player)
{
	using Direct3D9::Color;

	if (player->IsDormant())
		return;

	if (player->m_iLifeState() != LIFE_ALIVE)
		return;

	if (Config::ESP->Target == 0)
	{
		if (player->m_iTeamNum() == C_CSPlayer::GetLocalPlayer()->m_iTeamNum())
			return;
	}
	else if (Config::ESP->Target == 1)
	{
		if (player->m_iTeamNum() != C_CSPlayer::GetLocalPlayer()->m_iTeamNum() && player == C_CSPlayer::GetLocalPlayer())
			return;
	}

	Vector3 bone;

	if (!player->GetHitboxVector(0, bone))
		return;

	Color color;

	if (player->m_iTeamNum() == T_TEAM)
	{
		color = Config::Colors->T_ESP_Normal;
	}
	else if (player->m_iTeamNum() == CT_TEAM)
	{
		color = Config::Colors->CT_ESP_Normal;
	}

	const auto& transform = player->m_rgflCoordinateFrame();

	Vector3 position(transform[0][3], transform[1][3], transform[2][3]);

	Vector3 mins = player->GetMins();
	Vector3 maxs = player->GetMaxs() + Vector3(0.0f, 0.0f, 10.0f);

	Vector3 points[] =
	{
		{ mins.x, mins.y, mins.z },
		{ mins.x, maxs.y, mins.z },
		{ maxs.x, maxs.y, mins.z },
		{ maxs.x, mins.y, mins.z },
		{ maxs.x, maxs.y, maxs.z },
		{ mins.x, maxs.y, maxs.z },
		{ mins.x, mins.y, maxs.z },
		{ maxs.x, mins.y, maxs.z },
	};

	Vector3 transformed[8];

	for (int i = 0; i < 8; i++)
		VectorTransform(points[i], transform, transformed[i]);

	Vector3 flb, brt, blb, frt, frb, brb, blt, flt;

	if (!Source::WorldToScreen(transformed[3], flb) ||
		!Source::WorldToScreen(transformed[0], blb) ||
		!Source::WorldToScreen(transformed[2], frb) ||
		!Source::WorldToScreen(transformed[6], blt) ||
		!Source::WorldToScreen(transformed[5], brt) ||
		!Source::WorldToScreen(transformed[4], frt) ||
		!Source::WorldToScreen(transformed[1], brb) ||
		!Source::WorldToScreen(transformed[7], flt))
		return;

	Vector3 screen[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for (int i = 0; i < 8; i++)
	{
		if (left > screen[i].x)
			left = screen[i].x;
		if (top < screen[i].y)
			top = screen[i].y;
		if (right < screen[i].x)
			right = screen[i].x;
		if (bottom > screen[i].y)
			bottom = screen[i].y;
	}

	int x = (int)std::round(left);
	int y = (int)std::round(bottom);

	int w = (int)std::round(right - left);
	int h = (int)std::round(top - bottom);

	int pad_h = 0;

	if (Config::ESP->Name)
	{
		player_info_t data;

		if (Source::m_pEngine->GetPlayerInfo(player->GetIndex(), &data))
			Source::m_pRenderer->DrawText(Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color::White, data.name);
	}

	if (Config::ESP->Weapon)
	{
		auto weapon = player->GetActiveWeapon();

		if (weapon)
		{
			const char* name = weapon->GetName();

			if (name)
				Source::m_pRenderer->DrawText(Source::m_hFont, x + w / 2, y + h + 2, FONT_ALIGN_CENTER_H, Color::White, "%s", (name + 7));
		}
	}
	
	if (true)
	{
		Vector3 screen;

		if (Source::WorldToScreen(Config::Main->Aimbot->spotl, screen))
			Source::m_pRenderer->DrawRectOut(screen.x - 1, screen.y - 1, 3, 3, Color::White);
	}

	if (Config::ESP->Health)
	{
		int health = player->m_iHealth();

		if (health > 100)
			health = 100;

		int size_h = (int)std::round(((h + 1) * health) / 100);
		int real_h = (h + 1) - size_h;

		if (health > 0)
		{
			Source::m_pRenderer->DrawRect(x + w + 11, y + 1, 4, h + 3, Color::Black);
			Source::m_pRenderer->DrawRect(x + w + 12, y + real_h + 2, 2, size_h, Color(255 - (health * 2.55f), health * 2.55f, 0));
			if(health < 100)
				Source::m_pRenderer->DrawText(Source::m_hFont, x + w + 12, y + real_h + 5, FONT_ALIGN_CENTER, Color::White, "%i", health);
		}
	}

	if (Config::ESP->Armor)
	{
		int armor = player->m_ArmorValue();

		if (armor > 100)
			armor = 100;

		int size_h = (int)std::round(((h + 1) * armor) / 100);
		int real_h = (h + 1) - size_h;

		if (armor > 0) 
		{
			Source::m_pRenderer->DrawRect(x + w + 3, y + 1, 4, h + 3, Color::Black);
			Source::m_pRenderer->DrawRect(x + w + 4, y + real_h + 2, 2, size_h, Color(180, 180, 180));
			if (armor < 100)Source::m_pRenderer->DrawText(Source::m_hFont, x + w + 4, y + real_h + 5, FONT_ALIGN_CENTER, Color::White, "%i", armor);
		}
		
	}
}

void ESP()
{
	auto player = C_CSPlayer::GetLocalPlayer();

	if (!player)
		return;

	int size = Source::m_pEntList->GetHighestEntityIndex();

	for (int i = 0; i <= size; i++)
	{
		auto ent = Source::m_pEntList->GetBaseEntity(i);

		if (!ent)
			continue;

		auto enemy = ToCSPlayer(ent);

		if (!enemy)
			continue;

		if (enemy == player)
		{
			if (Config::Misc->ThirdPerson && player->m_iLifeState() == LIFE_ALIVE && Source::m_pInput->m_fCameraInThirdPerson == true)
			{
				const auto& transform = player->m_rgflCoordinateFrame();

				Vector3 position(transform[0][3], transform[1][3], transform[2][3]);

				Vector3 mins = player->GetMins();
				Vector3 maxs = player->GetMaxs() + Vector3(0.0f, 0.0f, 10.0f);

				Vector3 points[] =
				{
					{ mins.x, mins.y, mins.z },
					{ mins.x, maxs.y, mins.z },
					{ maxs.x, maxs.y, mins.z },
					{ maxs.x, mins.y, mins.z },
					{ maxs.x, maxs.y, maxs.z },
					{ mins.x, maxs.y, maxs.z },
					{ mins.x, mins.y, maxs.z },
					{ maxs.x, mins.y, maxs.z },
				};

				Vector3 transformed[8];

				for (int i = 0; i < 8; i++)
					VectorTransform(points[i], transform, transformed[i]);

				Vector3 flb, brt, blb, frt, frb, brb, blt, flt;

				if (!Source::WorldToScreen(transformed[3], flb) ||
					!Source::WorldToScreen(transformed[0], blb) ||
					!Source::WorldToScreen(transformed[2], frb) ||
					!Source::WorldToScreen(transformed[6], blt) ||
					!Source::WorldToScreen(transformed[5], brt) ||
					!Source::WorldToScreen(transformed[4], frt) ||
					!Source::WorldToScreen(transformed[1], brb) ||
					!Source::WorldToScreen(transformed[7], flt))
					return;

				Vector3 screen[] = { flb, brt, blb, frt, frb, brb, blt, flt };

				float left = flb.x;
				float top = flb.y;
				float right = flb.x;
				float bottom = flb.y;

				for (int i = 0; i < 8; i++)
				{
					if (left > screen[i].x)
						left = screen[i].x;
					if (top < screen[i].y)
						top = screen[i].y;
					if (right < screen[i].x)
						right = screen[i].x;
					if (bottom > screen[i].y)
						bottom = screen[i].y;
				}

				int x = (int)std::round(left);
				int y = (int)std::round(bottom);

				int w = (int)std::round(right - left);
				int h = (int)std::round(top - bottom);
				using Direct3D9::Color;
				if (player->IsDormant())
					return;

				if (player->m_iLifeState() != LIFE_ALIVE)
					return;
			}
			else
			{
				continue;
			}
		}

		PaintEntity(enemy);
	}
}


void Watermark()
{
	static float rainbow; rainbow += 0.00025f; if
		(rainbow > 1.f) rainbow = 0.f;
	int w, h;

	Source::m_pEngine->GetScreenSize(w, h);
	
	Source::m_pRenderer->DrawText(Source::m_hFont, (float)w - 20.0f, 14.0f, FONT_ALIGN_RIGHT, Direct3D9::Color::White, XorStr("Global: %0.f"), Source::m_pGlobalVars->curtime);	
	Source::m_pRenderer->DrawText(Source::m_hFont, (float)w - 20.0f, 26.0f, FONT_ALIGN_RIGHT, Direct3D9::Color::White, XorStr("Time: %0.f"), Source::m_pAimbot->oldWeaponTime);	
	Source::m_pRenderer->DrawText(Source::m_hFont, (float)w - 20.0f, 46.0f, FONT_ALIGN_RIGHT, Direct3D9::Color::White, XorStr("Dif: %0.f"), Source::m_pGlobalVars->curtime - Source::m_pAimbot->oldWeaponTime);
	
}

void Hooked_Present_Body()
{
	__try
	{
		Source::m_pRenderer->Begin();
		if (!Shared::m_bPanic)
		{
			Watermark();

			if (Config::ESP->Enabled)
				ESP();
			if (Config::Misc->HitmarkerEnabled)
			{
				auto& hitmarker = Feature::HitMarker::Instance();
				hitmarker.Present();
			}
			
			Source::m_pMenu->OnPresentDevice();
			
			
		}
		Source::m_pRenderer->End();
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}
}

HRESULT D3DAPI Hooked_Present(IDirect3DDevice9* device, const RECT* source_rect, const RECT* dest_rect, HWND dest_window_override, const RGNDATA* dirty_region)
{
	Hooked_Present_Body();
	return Source::m_pDeviceSwap->VCall< PresentFn >(IDirect3DDevice9_Present)(device, source_rect, dest_rect, dest_window_override, dirty_region);
}

void Hooked_CL_RunPrediction(PREDICTION_REASON reason)
{
	static auto s_nSignonState = *(std::uintptr_t*)(Memory::PatternScan("engine.dll", "83 3D ?? ?? ?? ?? ?? A3 ?? ?? ?? ?? 75 47") + 2);
	static auto s_nDeltaTick = *(std::uintptr_t*)(Memory::PatternScan("engine.dll", "83 3D ?? ?? ?? ?? ?? 7C 3E 8B 0D") + 2);
	static auto s_last_command_ack = *(std::uintptr_t*)(Memory::PatternScan("engine.dll", "A1 ?? ?? ?? ?? 56 50 A1") + 1);
	static auto s_lastoutgoingcommand = *(std::uintptr_t*)(Memory::PatternScan("engine.dll", "A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B 11 53 56") + 1);
	static auto s_chokedcommands = *(std::uintptr_t*)(Memory::PatternScan("engine.dll", "8B 35 ?? ?? ?? ?? 03 F0 A1 ?? ?? ?? ??") + 2);

	int nSignonState = *(int*)s_nSignonState;
	int nDeltaTick = *(int*)s_nDeltaTick;
	int last_command_ack = *(int*)s_last_command_ack;
	int lastoutgoingcommand = *(int*)s_lastoutgoingcommand;
	Config::AntiAim->choked_commands = *(int*)s_chokedcommands;

	if (nSignonState != 6)
		return;

	if (nDeltaTick < 0)
		return;

	Source::m_pPrediction->Update(nDeltaTick, true, last_command_ack, lastoutgoingcommand + Config::AntiAim->choked_commands);
}

void DT_BasePlayer_m_nTickBase( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	*( int* )( pOut ) = pData->m_Value.m_Int;
	if( pStruct == C_CSPlayer::GetLocalPlayer() )
		Source::m_pDataManager->OnDataRecieved();
}

void DT_BasePlayer_m_vecPunchAngle( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	*( Vector3* )( pOut ) = pData->m_Value.m_Vector;
}

void DT_ParticleSmokeGrenade_m_flSpawnTime( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	float Value = pData->m_Value.m_Float;

	if( Config::Removals->NoSmoke)
		Value = 0.0f;

	*( float* )( pOut ) = Value;
}

bool hasaa;

void DT_CSPlayer_m_angEyeAnglesX(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	float angle = pData->m_Value.m_Float;
	hasaa = false;
	if (Config::Main->Aimbot->Resolver && Config::Main->Aimbot->Enabled)
	{
		C_CSPlayer* local = C_CSPlayer::GetLocalPlayer();
		if (!local)return;
		C_CSPlayer* player = (C_CSPlayer*)pStruct;
		if (!player)return;
		C_CSPlayer* aimbot_target = ToCSPlayer(Source::m_pEntList->GetBaseEntity(Config::Main->Aimbot->target_idx));
		if (!aimbot_target)return;
		if (local->m_iLifeState() == LIFE_DEAD && player && aimbot_target != local)
		{
			if (angle <= -539.0) angle = 89.0;
			if (angle >= 539.0) angle = -89.0;

			if (angle == 89.0 || angle == -89.0 || angle == 70.0 || angle == -70.0 || angle == 0)
				hasaa = true;
			else
				hasaa = false;
		}
	}

	*(float*)(pOut) = angle;
}


void DT_CSPlayer_m_angEyeAnglesY(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	float angle = pData->m_Value.m_Float;
	if (hasaa)
	{
		C_CSPlayer* local = C_CSPlayer::GetLocalPlayer();
		C_CSPlayer* player = (C_CSPlayer*)pStruct;
		C_CSPlayer* aimbot_target = ToCSPlayer(Source::m_pEntList->GetBaseEntity(Config::Main->Aimbot->target_idx));
		
	}
	*(float*)(pOut) = angle;
}
	