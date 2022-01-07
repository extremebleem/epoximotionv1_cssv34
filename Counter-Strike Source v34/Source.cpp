#include "Source.hpp"
#include "Hooked.hpp"
#include "Player.hpp"
#include "HitMarker.hpp"
#include "Feature.hpp"
#include "AntiAim.h"
#include "Misc.h"

class GameEventListener : public IGameEventListener2
{
public:
	void FireGameEvent(IGameEvent* game_event)
	{
		auto& hitmarker = Feature::HitMarker::Instance();
		hitmarker.FireGameEvent(game_event);
	}
};

GameEventListener g_GameEventListener = {};


namespace Source
{
	IBaseClientDLL* m_pClient = nullptr;
	IClientEntityList* m_pEntList = nullptr;
	IGameMovement* m_pGameMovement = nullptr;
	IPrediction* m_pPrediction = nullptr;
	IVEngineClient* m_pEngine = nullptr;
	IVModelInfoClient* m_pModelInfoClient = nullptr;
	IVRenderView* m_pRenderView = nullptr;
	IVModelRender* m_pModelRender = nullptr;
	IEngineTrace* m_pEngineTrace = nullptr;
	ICvar* m_pCvar = nullptr;
	IPhysicsSurfaceProps* m_pPhysicsSurfaceProps = nullptr;
	IMaterialSystem* m_pMatSystem = nullptr;
	IGameEventManager2* m_pGameEventManager = nullptr;

	CGlobalVars* m_pGlobalVars = nullptr;
	IInput* m_pInput = nullptr;

	IDirect3DDevice9* m_pDevice = nullptr;

	std::shared_ptr< Input::Win32 >				m_pTargetInput = nullptr;

	std::shared_ptr< Source::NetVarManager >	m_pNetVarManager = nullptr;
	std::shared_ptr< Direct3D9::Renderer >		m_pRenderer = nullptr;

	Direct3D9::HFont							m_hFont = INVALID_FONT_HANDLE;

	std::shared_ptr< Feature::Accuracy >		m_pAccuracy = nullptr;
	std::shared_ptr< Feature::Aimbot >			m_pAimbot = nullptr;
	std::shared_ptr< Feature::AntiAim >			m_pAntiAim = nullptr;

	std::shared_ptr< Feature::DataManager >		m_pDataManager = nullptr;
	std::shared_ptr< Feature::Menu >			m_pMenu = nullptr;
	std::shared_ptr< Feature::Render >			m_pRender = nullptr;
	std::shared_ptr< Feature::MiscClass >		m_pMisc = nullptr;

	std::shared_ptr< Memory::VmtSwap >			m_pClientSwap = nullptr;
	std::shared_ptr< Memory::VmtSwap >			m_pPredictionSwap = nullptr;
	std::shared_ptr< Memory::VmtSwap >			m_pInputSwap = nullptr;
	std::shared_ptr< Memory::VmtSwap >			m_pModelRenderSwap = nullptr;
	std::shared_ptr< Memory::VmtSwap >			m_pDeviceSwap = nullptr;

	std::shared_ptr< Memory::Detour >			m_pPresentSwap = nullptr;
	std::shared_ptr< Memory::Detour >			m_pRunPredictionSwap = nullptr;

	RecvVarProxyFn								m_nTickBase = nullptr;
	RecvVarProxyFn								m_vecPunchAngle = nullptr;
	RecvVarProxyFn								m_flSpawnTime = nullptr;
	RecvVarProxyFn								m_angEyeAnglesX = nullptr;
	RecvVarProxyFn								m_angEyeAnglesY = nullptr;

	bool Startup()
	{
		m_pClient = (IBaseClientDLL*)QueryInterface(XorStr("client.dll"), XorStr("VClient"));

		if (!m_pClient)
			return false;
		
		m_pEntList = (IClientEntityList*)QueryInterface(XorStr("client.dll"), XorStr("VClientEntityList"));

		if (!m_pEntList)
			return false;
		

		m_pGameMovement = (IGameMovement*)QueryInterface(XorStr("client.dll"), XorStr("GameMovement"));

		if (!m_pGameMovement)
			return false;
		

		m_pPrediction = (IPrediction*)QueryInterface(XorStr("client.dll"), XorStr("VClientPrediction"));

		if (!m_pPrediction)
			return false;
		

		m_pEngine = (IVEngineClient*)QueryInterface(XorStr("engine.dll"), XorStr("VEngineClient"));

		if (!m_pEngine)
			return false;
		

		m_pModelInfoClient = (IVModelInfoClient*)QueryInterface(XorStr("engine.dll"), XorStr("VModelInfoClient"));

		if (!m_pModelInfoClient)
			return false;
		

		m_pRenderView = (IVRenderView*)QueryInterface(XorStr("engine.dll"), XorStr("VEngineRenderView"));

		if (!m_pRenderView)
			return false;
		

		m_pModelRender = (IVModelRender*)QueryInterface(XorStr("engine.dll"), XorStr("VEngineModel"));

		if (!m_pModelRender)
			return false;
		

		m_pEngineTrace = (IEngineTrace*)QueryInterface(XorStr("engine.dll"), XorStr("EngineTraceClient"));

		if (!m_pEngineTrace)
			return false;
		

		m_pCvar = (ICvar*)QueryInterface(XorStr("engine.dll"), XorStr("VEngineCvar003"), true);

		if (!m_pCvar)
			return false;
		

		m_pPhysicsSurfaceProps = (IPhysicsSurfaceProps*)QueryInterface(XorStr("vphysics.dll"), XorStr("VPhysicsSurfaceProps"));

		if (!m_pPhysicsSurfaceProps)
			return false;
		

		m_pMatSystem = (IMaterialSystem*)QueryInterface(XorStr("materialsystem.dll"), XorStr("VMaterialSystem"));

		if (!m_pMatSystem)
			return false;

		m_pGameEventManager = (IGameEventManager2*)QueryInterface(XorStr("engine.dll"), XorStr("GAMEEVENTSMANAGER002"), true);

		if (!m_pGameEventManager)
			return false;

		m_pGlobalVars = **(CGlobalVars***)(Memory::PatternScan(XorStr("client.dll"), XorStr("A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 54 24 34")) + 1);

		if (!m_pGlobalVars)
			return false;

		m_pInput = **(IInput***)(Memory::PatternScan(XorStr("client.dll"), XorStr("8B 0D ?? ?? ?? ?? 8B 11 50 8B 44 24 10")) + 2);

		if (!m_pInput)
			return false;

		m_pDevice = Valve::Dx9Device()->m_pD3DDevice;

		if (!m_pDevice)
			return false;

		m_pTargetInput = std::make_shared< Input::Win32 >();

		if (!m_pTargetInput->Capture())
			return false;

		m_pNetVarManager = std::make_shared< Source::NetVarManager >();

		if (!m_pNetVarManager->Create(m_pClient))
			return false;

		m_pRenderer = std::make_shared< Direct3D9::Renderer >();

		if (!m_pRenderer->Create(m_pDevice))
			return false;

		m_hFont = m_pRenderer->CreateFont(XorStr("Tahoma"), 10, FONT_CREATE_BOLD | FONT_CREATE_DROPSHADOW);

		if (m_hFont == INVALID_FONT_HANDLE)
			return false;

		m_pAccuracy = std::make_shared< Feature::Accuracy >();
		m_pAimbot = std::make_shared< Feature::Aimbot >();
		m_pMisc = std::make_shared< Feature::MiscClass >();
		m_pAntiAim = std::make_shared< Feature::AntiAim >();
		
		m_pDataManager = std::make_shared< Feature::DataManager >();
		
		m_pMenu = std::make_shared< Feature::Menu >();
		m_pRender = std::make_shared< Feature::Render >();

		if (!m_pMenu->Create(m_pTargetInput->GetTarget(), m_pDevice))
			return false;

		//m_pGameEventManager->AddListener(&g_GameEventListener, "player_hurt", false);
		
		m_pClientSwap = std::make_shared< Memory::VmtSwap >();
		m_pPredictionSwap = std::make_shared< Memory::VmtSwap >();
		m_pInputSwap = std::make_shared< Memory::VmtSwap >();
		m_pModelRenderSwap = std::make_shared< Memory::VmtSwap >();
		m_pDeviceSwap = std::make_shared< Memory::VmtSwap >();

		m_pRunPredictionSwap = std::make_shared< Memory::Detour >();

		if (!m_pClientSwap->Apply(m_pClient))
			return false;
		
		if (!m_pPredictionSwap->Apply(m_pPrediction))
			return false;
		
		if (!m_pInputSwap->Apply(m_pInput))
			return false;

		if (!m_pModelRenderSwap->Apply(m_pModelRender))
			return false;
		
		if (!m_pDeviceSwap->Apply(m_pDevice))
			return false;

		uintptr_t CL_RunPrediction = Memory::PatternScan("engine.dll", "A1 ?? ?? ?? ?? 39 05 ?? ?? ?? ?? 74 55");

		if (!m_pRunPredictionSwap->Apply(CL_RunPrediction, (std::uintptr_t)Hooked_CL_RunPrediction, 5))
			return false;

		m_pClientSwap->Hook(&Hooked_CreateMove, IBaseClientDLL_CreateMove);
		m_pClientSwap->Hook(&Hooked_FrameStageNotify, IBaseClientDLL_FrameStageNotify);

		m_pPredictionSwap->Hook(&Hooked_RunCommand, IPrediction_RunCommand);
		m_pPredictionSwap->Hook(&Hooked_FinishMove, IPrediction_FinishMove);
		m_pPredictionSwap->Hook(&Hooked_Update, IPrediction_Update);

		m_pInputSwap->Hook(&Hooked_GetUserCmd, IInput_GetUserCmd);
		m_pInputSwap->Hook(&Hooked_ResetMouse, IInput_ResetMouse);

		m_pModelRenderSwap->Hook(&Hooked_DrawModelEx, IVModelRender_DrawModelEx);

		m_pDeviceSwap->Hook(&Hooked_Reset, IDirect3DDevice9_Reset);
		m_pDeviceSwap->Hook(&Hooked_Present, IDirect3DDevice9_Present);

		m_nTickBase = m_pNetVarManager->HookProp(XorStr("DT_BasePlayer"), XorStr("m_nTickBase"), DT_BasePlayer_m_nTickBase);
		m_vecPunchAngle = m_pNetVarManager->HookProp(XorStr("DT_BasePlayer"), XorStr("m_vecPunchAngle"), DT_BasePlayer_m_vecPunchAngle);
		m_flSpawnTime = m_pNetVarManager->HookProp(XorStr("DT_ParticleSmokeGrenade"), XorStr("m_flSpawnTime"), DT_ParticleSmokeGrenade_m_flSpawnTime);
		m_angEyeAnglesX = m_pNetVarManager->HookProp(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles[0]"), DT_CSPlayer_m_angEyeAnglesX);
		m_angEyeAnglesY = m_pNetVarManager->HookProp(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles[1]"), DT_CSPlayer_m_angEyeAnglesY);

		return true;
	}

	bool Release()
	{
		if (m_pTargetInput)
			if (!m_pTargetInput->Release())
				return false;

		if (m_pClientSwap)
			m_pClientSwap->Release();

		if (m_pPredictionSwap)
			m_pPredictionSwap->Release();

		if (m_pInputSwap)
			m_pInputSwap->Release();

		if (m_pModelRenderSwap)
			m_pModelRenderSwap->Release();

		if (m_pDeviceSwap)
			m_pDeviceSwap->Release();

		if (m_pPresentSwap)
			m_pPresentSwap->Release();

		if (m_pRunPredictionSwap)
			m_pRunPredictionSwap->Release();

		if (m_nTickBase)
		{
			m_pNetVarManager->HookProp(XorStr("DT_BasePlayer"), XorStr("m_nTickBase"), m_nTickBase);
			m_nTickBase = nullptr;
		}

		if (m_vecPunchAngle)
		{
			m_pNetVarManager->HookProp(XorStr("DT_BasePlayer"), XorStr("m_vecPunchAngle"), m_vecPunchAngle);
			m_vecPunchAngle = nullptr;
		}

		if (m_flSpawnTime)
		{
			m_pNetVarManager->HookProp(XorStr("DT_ParticleSmokeGrenade"), XorStr("m_flSpawnTime"), m_flSpawnTime);
			m_flSpawnTime = nullptr;
		}

		if (m_angEyeAnglesX)
		{
			m_pNetVarManager->HookProp(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles[0]"), m_angEyeAnglesX);
			m_angEyeAnglesX = nullptr;
		}

		if (m_angEyeAnglesY)
		{
			m_pNetVarManager->HookProp(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles[1]"), m_angEyeAnglesY);
			m_angEyeAnglesY = nullptr;
		}

		return true;
	}

	void Free()
	{
		// hooks
		Memory::SafeReset(m_pClientSwap);
		Memory::SafeReset(m_pPredictionSwap);
		Memory::SafeReset(m_pInputSwap);
		Memory::SafeReset(m_pModelRenderSwap);
		Memory::SafeReset(m_pDeviceSwap);
		Memory::SafeReset(m_pPresentSwap);
		Memory::SafeReset(m_pRunPredictionSwap);

		// framework
		Memory::SafeReset(m_pTargetInput);
		Memory::SafeReset(m_pNetVarManager);
		Memory::SafeReset(m_pRenderer);

		// features
		Memory::SafeReset(m_pAccuracy);
		Memory::SafeReset(m_pAimbot);
		Memory::SafeReset(m_pAntiAim);
		Memory::SafeReset(m_pMisc);

		Memory::SafeReset(m_pDataManager);
		
		Memory::SafeReset(m_pMenu);
		Memory::SafeReset(m_pRender);
	}

	void* QueryInterface(const char* szMod, const char* szName, bool bCustom)
	{
		auto hMod = ModuleD(szMod);

		if (!hMod)
			return nullptr;
		

		auto pCreateInterface = (CreateInterfaceFn)GetProcedure(hMod, XorStr("CreateInterface"));

		if (!pCreateInterface)
			return nullptr;
		

		if (bCustom)
			return pCreateInterface(szName, nullptr);

		char szFormat[1024];

		for (int i = 0; i < 1000; i++)
		{
			Crypt::sprintf_s(szFormat, sizeof(szFormat), XorStr("%s%03i"), szName, i);

			auto pRet = pCreateInterface(szFormat, nullptr);

			if (pRet)
				return pRet;
		}

		return nullptr;
	}
	
	void MovementFix(CUserCmd* cmd, const Vector3& va)
	{
		float yaw, speed;

		Vector3& move = *(Vector3*)&cmd->forwardmove;

		speed = move.Length2D();

		yaw = ToDegrees(atan2(move.y, move.x));
		yaw = ToRadians(cmd->viewangles.y - va.y + yaw);

		if (cmd->viewangles.x > 90.00 || cmd->viewangles.x < -90.00)
			move.x = -cos(yaw) * speed;
		else
			move.x = cos(yaw) * speed;

		move.y = sin(yaw) * speed;
	}

	bool TraceLine(const Vector3& vEnd, C_BaseEntity* pEnt)
	{
		auto player = C_CSPlayer::GetLocalPlayer();

		if (!player)
			return false;

		Ray_t ray;
		trace_t tr;

		CTraceFilterSimple trace(player);

		ray.Set(player->EyePosition(), vEnd);

		m_pEngineTrace->TraceRay(ray, 0x46004003, &trace, &tr);

		if (pEnt)
			return (tr.fraction == 1.0f || tr.m_pEnt == pEnt);

		return (tr.fraction == 1.0f);
	}

	bool WorldToScreen(const Vector3& vPoint, Vector3& vOut)
	{
		auto vMatrix = m_pEngine->WorldToScreenMatrix();

		vOut.x = vMatrix[0][0] * vPoint.x + vMatrix[0][1] * vPoint.y + vMatrix[0][2] * vPoint.z + vMatrix[0][3];
		vOut.y = vMatrix[1][0] * vPoint.x + vMatrix[1][1] * vPoint.y + vMatrix[1][2] * vPoint.z + vMatrix[1][3];

		auto w = vMatrix[3][0] * vPoint.x + vMatrix[3][1] * vPoint.y + vMatrix[3][2] * vPoint.z + vMatrix[3][3];

		if (w < 0.01f)
			return false;

		auto invw = 1.0f / w;

		vOut.x *= invw;
		vOut.y *= invw;

		int width, height;

		m_pEngine->GetScreenSize(width, height);

		float x = (float)(width / 2);
		float y = (float)(height / 2);

		x += 0.5f * vOut.x * width + 0.5f;
		y -= 0.5f * vOut.y * height + 0.5f;

		vOut.x = x;
		vOut.y = y;

		return true;
	}
}