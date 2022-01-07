#include "Render.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Config.hpp"

namespace Feature
{
	Render::Render()
	{
		
	}

	void Render::OnDrawModel(void* ecx, ModelRenderInfo_t* info)
	{
		C_CSPlayer* player = C_CSPlayer::GetLocalPlayer();

		if (!player)return;

		C_CSPlayer* enemy = ToCSPlayer(Source::m_pEntList->GetBaseEntity(info->ent_index));

		if (!enemy)return;

		if (enemy->m_iLifeState() != LIFE_ALIVE)return;

		if (player == enemy && Config::Render->ChamsMode[0] != 0)
		{

			static IMaterial* pIn = CreateMaterial(Config::Render->ChamsMode[0] == 1 ? false : true, false);
			static IMaterial* pOut = CreateMaterial(Config::Render->ChamsMode[0] == 1 ? false : true, false);
		
			Color color = Config::Colors->LOCAL_Chams_Outline;

			static IMaterial* pOutline = CreateMaterial(false, true, true);

			if (Config::Render->ChamsOutlined[0])
			{

				ForceMaterial(color, pOutline);

				//Source::m_pModelRenderSwap->VCall< DrawModelExFn >(IVModelRender_DrawModelEx)(ecx, info);
			}

			color = Config::Colors->LOCAL_Chams;

			ForceMaterial(color, pIn);
		}

		if (player->m_iTeamNum() == enemy->m_iTeamNum() && player != enemy && Config::Render->ChamsMode[2] != 0)
		{

			static IMaterial* pIn = CreateMaterial(Config::Render->ChamsMode[2] == 1 ? false : true, true);
			static IMaterial* pOut = CreateMaterial(Config::Render->ChamsMode[2] == 1 ? false : true, false);
			

			Color color = Config::Colors->TEAM_Chams_Outline;

			static IMaterial* pOutline = CreateMaterial(false, true, true);

			if (Config::Render->ChamsOutlined[2])
			{

				ForceMaterial(color, pOutline);

				//Source::m_pModelRenderSwap->VCall< DrawModelExFn >(IVModelRender_DrawModelEx)(ecx, info);
			}

			color = Config::Colors->TEAM_Chams_Normal;

			ForceMaterial(color, pIn);

			//Source::m_pModelRenderSwap->VCall< DrawModelExFn >(IVModelRender_DrawModelEx)(ecx, info);

			color = Config::Colors->TEAM_Chams_Colored;
			
			ForceMaterial(color, pOut);
		}

		if (player->m_iTeamNum() != enemy->m_iTeamNum() && Config::Render->ChamsMode[1] != 0)
		{
			static IMaterial* pIn = CreateMaterial(Config::Render->ChamsMode[1] == 1 ? false : true, true);
			static IMaterial* pOut = CreateMaterial(Config::Render->ChamsMode[1] == 1 ? false : true, false);
			
			Color color = Config::Colors->ENEMY_Chams_Outline;

			static IMaterial* pOutline = CreateMaterial(false, true, true);

			if (Config::Render->ChamsOutlined[1])
			{
				
				
				ForceMaterial(color, pOutline);

				Source::m_pModelRenderSwap->VCall< DrawModelExFn >(IVModelRender_DrawModelEx)(ecx, info);
			}

			color = Config::Colors->ENEMY_Chams_Normal;

			ForceMaterial(color, pIn);

			Source::m_pModelRenderSwap->VCall< DrawModelExFn >(IVModelRender_DrawModelEx)(ecx, info);

			color = Config::Colors->ENEMY_Chams_Colored;
			
			ForceMaterial(color, pOut);
		}
	}

	IMaterial* Render::CreateMaterial(bool bVertexLit, bool bIgnoreZ, bool bWireframe)
	{
		static int iCreated = 0;

		static const char tmp[] =
		{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
			\n\t\"$envmap\" \"\"\
			\n\t\"$model\" \"1\"\
			\n\t\"$receiveflashlight\" \"1\"\
			\n\t\"$singlepassflashlight\" \"1\"\
			\n\t\"$flat\" \"1\"\
			\n\t\"$nocull\" \"0\"\
			\n\t\"$selfillum\" \"1\"\
			\n\t\"$halflambert\" \"1\"\
			\n\t\"$nofog\" \"0\"\
			\n\t\"$ignorez\" \"%i\"\
			\n\t\"$znearer\" \"0\"\
			\n\t\"$wireframe\" \"%i\"\
        \n}\n"
		};

		const char* szBaseType = bVertexLit ? "VertexLitGeneric" : "UnlitGeneric";

		char szMaterial[512];
		sprintf_s(szMaterial, sizeof(szMaterial), tmp, szBaseType, bIgnoreZ ? 1 : 0, bWireframe ? 1 : 0);

		char szName[512];
		sprintf_s(szName, sizeof(szName), "chams_%i.vmt", iCreated);

		iCreated++;

		KeyValues* pKey = new KeyValues(szBaseType);

		pKey->LoadFromBuffer(szName, szMaterial);

		IMaterial* pMat = Source::m_pMatSystem->CreateMaterial(szName, pKey);

		return pMat;
	}

	void Render::ForceMaterial(const Color& color, IMaterial* mat, bool mod)
	{
		if (mod)
		{
			float col[3] =
			{
				color.R / 255.0f,
				color.G / 255.0f,
				color.B / 255.0f
			};

			float alpha = color.A / 255.0f;

			Source::m_pRenderView->SetBlend(alpha);
			Source::m_pRenderView->SetColorModulation(col);
		}

		Source::m_pModelRender->ForcedMaterialOverride(mat);
	}
}
