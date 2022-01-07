
#include "Menu.hpp"
#include "Source.hpp"
#include "Config.hpp"
#include "Player.hpp"
#include "ImGui\imgui.h"
#include "ImGUI\dx9\imgui_impl_dx9.h"
#include <ctime>

int iTab = 0;
int iWeaponTab = 1;
float menusize = 50.0f;
bool open;
float mainmenu1 = 740.0f;
float mainmenu2 = 480.0f;
float backsize1 = mainmenu1+10;
float backsize2 = mainmenu2+10;

ImFont*  bad = nullptr; 
ImFont*  MenuFont = nullptr; 
ImFont*  EpoxiFont = nullptr; 
ImFont*  ButtonFont = nullptr; 
ImFont*  ManualFont = nullptr; 
ImFont*  WeaponFont = nullptr; 

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const char* colorlist[] =
{
	"Enemy",
	"Enemy Wall",
	"Enemy Outline",
	"Team",
	"Team Wall",
	"Team Outline",
	"Local",
	"Local Outline",
	"Weapon",
	"Weapon Wall"
};

const char* NoEnemyList[] =
{
	"Everyone",
	"Enemy",
	"Friendly",
};

const char* FakeList[] =
{
	"Standart",
	"Adaptive"
};

const char* EspTargetList[] =
{
	"Enemy",
	"Friendly",
};

const char* InfoTypeList[] =
{
	"Off",
	"Text",
	"Bar",
};

const char* InfoAlignList[] =
{
	"Left",
	"Right",
	"Top",
	"Bottom",
};

const char* ChamsModeList[] =
{
	"Off",
	"Flat",
	"Shadow"
};

const char* ChamsTargetList[] =
{
	"Local",
	"Enemy",
	"Team"
};

const char* AutoStrafeList[] =
{
	"Off",
	"Mouse",
	"Auto",
};

const char* WeaponTypeMenu[] =
{
	"none",
	"PISTOLS",
	"HEAVY",
	"RIFLES",
	"SCOUT",
	"AWP",
	"AUTO",
	"AUTO"
};

namespace Feature
{
	Menu::Menu()
	:	m_bMouse(false),
		m_iWeaponAimbot(0),
		m_iWeaponTriggerbot(0),
		m_iConfig(-1),
		m_bAimbot(false),
		m_bESP(false),
		m_bAntiAim(false),
		m_bRemovals(false),
		m_bMisc(false),
		m_bColors(false),
		m_bBinds(false),
		m_bConfig(false)
	{
		ZeroMemory(m_szConfigName, sizeof(m_szConfigName));
	}

	Menu::~Menu()
	{
		Source::m_pCvar->FindVar(XorStr("cl_mouseenable"))->m_nValue = 1;

		ImGui_ImplDX9_Shutdown();
	}

	bool Menu::Create(HWND hWnd, IDirect3DDevice9* pDevice)
	{
		
		if (!ImGui_ImplDX9_Init(hWnd, pDevice))
			return false;

		ImGuiIO& io = ImGui::GetIO();

		char WindowsPath[MAX_PATH];
		char FirstFont[MAX_PATH] = "";
		char SecondFont[MAX_PATH] = "";
		GetWindowsDirectory(WindowsPath, MAX_PATH);
		strcat(FirstFont, WindowsPath);
		strcat(FirstFont, "\\INF\\first1.inf");
		strcat(SecondFont, WindowsPath);
		strcat(SecondFont, "\\INF\\first2.inf");
		MenuFont = io.Fonts->AddFontFromFileTTF(FirstFont, 18.0f);
		EpoxiFont = io.Fonts->AddFontFromFileTTF(SecondFont, 26.0f);
		ButtonFont = io.Fonts->AddFontFromFileTTF(SecondFont, 20.0f);


		//ManualFont = io.Fonts->AddFontFromFileTTF(a[0] + ":\\Windows\\Fonts\\PIZZADUDEPOINTERS.ttf", 22.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
		
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 2.f;
		style.ChildWindowRounding = 5.f;
		style.FramePadding = ImVec2(8, 0);
		style.WindowPadding = ImVec2(4, 0);
		style.ItemSpacing = ImVec2(0, 0);
		style.ScrollbarSize = 10.f;
		style.ScrollbarRounding = 0.f;
		style.GrabMinSize = 5.f;

		//SetColors();

		return true;
	}

	void ButtonColor(int ra, int ga, int ba, int rh, int gh, int bh, int r, int g, int b)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_Button] = ImColor(r, g, b);
		style.Colors[ImGuiCol_ButtonHovered] = ImColor(rh, gh, bh);
		style.Colors[ImGuiCol_ButtonActive] = ImColor(ra, ga, ba);

	}

	std::string GetExtension(const std::string& target)
	{
		auto found = target.find_last_of(".");

		std::string extension("");

		if (found != std::string::npos)
			extension = target.substr(found + 1, target.length());

		return extension;
	}
	
	void styled()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 5.f;
		style.FramePadding = ImVec2(0, 0);
		style.WindowPadding = ImVec2(0, 0);
		style.ItemSpacing = ImVec2(0, 0);
		style.ScrollbarSize = 10.f;
		style.ScrollbarRounding = 0.f;
		style.GrabMinSize = 5.f;
	}

	void SameLine(float x = 5)
	{
		
		ImGui::Dummy(ImVec2(x, 0));
		ImGui::SameLine();
	}

	void Spacing(float y = 3)
	{
		ImGui::Dummy(ImVec2(0, y));
	}

	void BothXY(float x = 5, float y = 3)
	{
		Spacing(y);
		SameLine(x);
	}

	void Menu::OnPresentDevice()
	{
		
		if (m_bMouse != Shared::m_bMenu)
		{
			m_bMouse = Shared::m_bMenu;

			Source::m_pCvar->FindVar(XorStr("cl_mouseenable"))->m_nValue = (int)!m_bMouse;
		}
		
		if (!Shared::m_bMenu)return;

		ImGui_ImplDX9_NewFrame();
		ImGui::GetIO().MouseDrawCursor = true;
		ImGuiStyle& style = ImGui::GetStyle();

		ImVec2 pos;
		ImGuiWindowFlags Flags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

		ImGui::SetNextWindowSize(ImVec2(mainmenu1, mainmenu2));
		
		style.WindowPadding = ImVec2(0, 0);
		style.ItemSpacing = ImVec2(0, 0);
		style.FrameRounding = 0.0f;
		style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255);
		style.Colors[ImGuiCol_TextDisabled] = ImColor(0, 112, 253);
		style.Colors[ImGuiCol_ChildWindowBg] = ImColor(30, 30, 30); // Sub Tabs Background
		style.Colors[ImGuiCol_WindowBg] = ImColor(52, 52, 52); // Main BackGround
		style.Colors[ImGuiCol_PopupBg] = ImColor(52, 52, 52);
		style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
		style.Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
		style.Colors[ImGuiCol_FrameBg] = ImColor(52, 52, 52);
		style.Colors[ImGuiCol_FrameBgHovered] = ImColor(70, 70, 170);
		style.Colors[ImGuiCol_FrameBgActive] = ImColor(12, 66, 200);
		style.Colors[ImGuiCol_MenuBarBg] = ImColor(125, 52, 52);
		style.Colors[ImGuiCol_SliderGrabActive] = ImColor(12, 66, 200);

		ButtonColor(12, 66, 200, 70, 70, 170, 52, 52, 52);
		ImGui::Begin("mainmenu", NULL, ImVec2(mainmenu1, mainmenu2), 0.9f, Flags);
		{
			pos = ImGui::GetWindowPos();
			static ImVec2 posstat = ImGui::GetWindowPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			ImGui::PushFont(EpoxiFont);
			ButtonColor(52, 52, 52, 52, 52, 52, 52, 52, 52);
			ImGui::Button(XorStr("EPOXIMOTION"), ImVec2(180, 60));
			ImGui::PopFont();
			ImGui::PushFont(ButtonFont);
			ImGui::SameLine();
			if (iTab == 1)
				ButtonColor(12, 66, 200, 12, 66, 200, 12, 66, 200);
			else
				ButtonColor(12, 66, 200, 70, 70, 170, 52, 52, 52);
			if (ImGui::Button("AIMBOT", ImVec2(140, 60)))	iTab = 1;
			ImGui::SameLine();
			if (iTab == 2)
				ButtonColor(12, 66, 200, 12, 66, 200, 12, 66, 200);
			else
				ButtonColor(12, 66, 200, 70, 70, 170, 52, 52, 52);
			if (ImGui::Button("ANTIAIM", ImVec2(140, 60)))	iTab = 2;
			ImGui::SameLine();
			if (iTab == 3)
				ButtonColor(12, 66, 200, 12, 66, 200, 12, 66, 200);
			else
				ButtonColor(12, 66, 200, 70, 70, 170, 52, 52, 52);
			if (ImGui::Button("VISUALS", ImVec2(140, 60)))	iTab = 3;
			ImGui::SameLine();
			if (iTab == 4)
				ButtonColor(12, 66, 200, 12, 66, 200, 12, 66, 200);
			else
				ButtonColor(12, 66, 200, 70, 70, 170, 52, 52, 52);
			if (ImGui::Button("MISC", ImVec2(140, 60)))		iTab = 4;
			draw_list->AddRectFilled(ImVec2(pos.x, pos.y + 60), ImVec2(pos.x + 740, pos.y + 68), ImColor(12, 66, 200));
			
			if (iTab == 0)
			{
				ImGui::Columns(4, nullptr, false);
				ImGui::SetColumnOffset(1, 10);
				ImGui::SetColumnOffset(2, 330);
				ImGui::SetColumnOffset(3, 410);

				ImGui::NextColumn();
				ImGui::Dummy(ImVec2(0, 20));
				style.Colors[ImGuiCol_ChildWindowBg] = ImColor(52,52,52); // Sub Tabs Background
				ImGui::BeginChild("info_warning", ImVec2(600, 390), false);
				{
					static ImVec2 posstat = ImGui::GetWindowPos();
					ImVec2 pos = ImGui::GetWindowPos();
					
					BothXY(5, 7);
					ImGui::Text("WARNING!"); BothXY();

					ImGui::Text("It's a beta version."); BothXY();

					ImGui::Text("If you find any bug write to us: "); BothXY();

					ImGui::Text("vk.com/epoximotion\n ");
				}ImGui::EndChild();
			}
			ImGui::PopFont();
			ImGui::PushFont(MenuFont);
			style.Colors[ImGuiCol_ChildWindowBg] = ImColor(30, 30, 30); // Sub Tabs Background
			if (iTab == 1)
			{

				ImGui::Columns(4, nullptr, false);
				ImGui::SetColumnOffset(1, 10);
				ImGui::SetColumnOffset(2, 330);
				ImGui::SetColumnOffset(3, 410);


				ImGui::NextColumn();

				ImGui::Dummy(ImVec2(0, 20));
				ImGui::BeginChild("weapon_general", ImVec2(320, 390), false);
				{
					static ImVec2 posstat = ImGui::GetWindowPos();
					ImVec2 pos = ImGui::GetWindowPos();

					//General

					BothXY(5, 7);
					ImGui::Checkbox("Enabled", &Config::Main->Aimbot->Enabled);					BothXY();
					ImGui::Checkbox("Resolver", &Config::Main->Aimbot->Resolver);					BothXY();
					ImGui::Checkbox("Perfect Silent", &Config::Main->Aimbot->PSilent);					BothXY();
					ImGui::Checkbox("Remove Recoil", &Config::Removals->NoRecoil); BothXY();
					ImGui::Checkbox("Remove Visual Recoil", &Config::Removals->NoVisualRecoil); BothXY();
					ImGui::Checkbox("Remove Spread", &Config::Main->Aimbot->NoSpreadActive); BothXY();
					ImGui::SliderInt("Min.Damage:", &Config::Main->Aimbot->MinDamage, 0, 100);
					
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 385), ImVec2(pos.x + 320, pos.y + 390), ImColor(0, 112, 253));
				
				}ImGui::EndChild();
				//ImGui::NextColumn();

				/*ImGui::Dummy(ImVec2(0, 285));
				if (ImGui::Button("PISTOLS", ImVec2(80, 20)))	iWeaponTab = 1;

				if (ImGui::Button("HEAVY", ImVec2(80, 20)))	iWeaponTab = 2;

				if (ImGui::Button("RIFLES", ImVec2(80, 20)))	iWeaponTab = 3;

				if (ImGui::Button("SCOUT", ImVec2(80, 20)))	iWeaponTab = 4;

				if (ImGui::Button("AWP", ImVec2(80, 20)))	iWeaponTab = 5;

				if (ImGui::Button("AUTO", ImVec2(80, 20)))	iWeaponTab = 6;*/

				/*ImGui::NextColumn();

				ImGui::Dummy(ImVec2(0, 20));
				ImGui::BeginChild("weapon_override", ImVec2(320, 390), false, Flags);
				{
					static ImVec2 posstat = ImGui::GetWindowPos();
					ImVec2 pos = ImGui::GetWindowPos();

					//General

					BothXY(5, 7);
					//ImGui::Checkbox("Override Min.Damage", &Config::WeaponType[iWeaponTab]->Aimbot->MinDamageOverride);
					if (Config::WeaponType[iWeaponTab]->Aimbot->MinDamageOverride)
					{
						BothXY();
						ImGui::SliderInt("Min.Damage:", &Config::WeaponType[iWeaponTab]->Aimbot->MinDamage, 0, 150);
					}
					//if (iWeaponTab == 1)
					//{

						//BothXY();
						
					/*}
					BothXY();
					ImGui::Checkbox("Auto Stop", &Config::WeaponType[iWeaponTab]->Aimbot->AutoStop);
					if (iWeaponTab == 4 || iWeaponTab == 5 || iWeaponTab == 6)
					{
						BothXY();
						ImGui::Checkbox("Auto Scope", &Config::WeaponType[iWeaponTab]->Aimbot->AutoScope);
					}//

					static bool open = false;
					ImGui::GetStyle().FrameRounding = 2.5f;
					BothXY();
					if (ImGui::Button("Hitboxes"))open = !open;


					if (open)
					{

						BothXY(10, 2);
						ImGui::Selectable("Head", &Config::Main->Aimbot->HitscanBone[1], 0, ImVec2(60, 20));

						BothXY(10, 0);
						ImGui::Selectable("Pelvis", &Config::Main->Aimbot->HitscanBone[3], 0, ImVec2(60, 20));

						BothXY(10, 0);
						ImGui::Selectable("Stomach", &Config::Main->Aimbot->HitscanBone[4], 0, ImVec2(60, 20));

						BothXY(10, 0);
						ImGui::Selectable("Chest", &Config::Main->Aimbot->HitscanBone[5], 0, ImVec2(60, 20));

						BothXY(10, 0);
						ImGui::Selectable("Hands", &Config::Main->Aimbot->HitscanBone[6], 0, ImVec2(60, 20));

						BothXY(10, 0);
						ImGui::Selectable("Foots", &Config::Main->Aimbot->HitscanBone[7], 0, ImVec2(60, 20));

					}



					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 385), ImVec2(pos.x + 320, pos.y + 390), ImColor(0, 112, 253));
					
				}ImGui::EndChild();*/
				Config::WeaponType[iWeaponTab]->Aimbot->Clamp();
				Config::Main->Aimbot->Clamp();
			}
			if (iTab == 2)
			{
				ImGui::Columns(4, nullptr, false);
				ImGui::SetColumnOffset(1, 10);
				ImGui::SetColumnOffset(2, 330);
				ImGui::SetColumnOffset(3, 410);

				
				ImGui::NextColumn();

				ImGui::Dummy(ImVec2(0, 20));
				ImGui::BeginChild("antiaim_general", ImVec2(320, 390), false, Flags);
				{
					static ImVec2 posstat = ImGui::GetWindowPos();
					ImVec2 pos = ImGui::GetWindowPos();

					
					BothXY(5, 7);
					ImGui::Checkbox("Enabled", &Config::AntiAim->Enabled);

					BothXY();
					ImGui::Checkbox("At Target", &Config::AntiAim->AtTargetEnabled);

					BothXY();
					ImGui::Checkbox("Enable experimental jitters", &Config::AntiAim->EnabledEx);

					BothXY();
					ImGui::Checkbox("Manual directions", &Config::AntiAim->EnableManuals);
					
					if (Config::AntiAim->EnableManuals)
					{
						BothXY();
						//ImGui::KeyButton("Left", &Config::AntiAim->leftkey); BothXY();
						//ImGui::KeyButton("Back", &Config::AntiAim->backwardkey); BothXY();
						//ImGui::KeyButton("Right", &Config::AntiAim->rightkey); BothXY();
						ImGui::Checkbox("Fake manuals", &Config::AntiAim->FakeManuals); BothXY();
					}
					else
						BothXY();

					BothXY();
					ImGui::Checkbox("Custom angles   ", &Config::AntiAim->CustomAngles); ImGui::SameLine();
					//ImGui::KeyButton("Flip", &Config::AntiAim->flipkey);
					if (Config::AntiAim->CustomAngles)
					{
						BothXY();
						ImGui::SliderInt("Real yaw: ",&Config::AntiAim->CustomAnglesR,0,180); BothXY();
						ImGui::SliderInt("Fake yaw: ",&Config::AntiAim->CustomAnglesF,-180,0);
						
					}

					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 385), ImVec2(pos.x + 320, pos.y + 390), ImColor(0, 112, 253));
					
				}ImGui::EndChild();
				ImGui::NextColumn();
				ImGui::NextColumn();
				ImGui::Dummy(ImVec2(0, 20));
				ImGui::BeginChild("antiaim_misc", ImVec2(320, 390), false, Flags);
				{
					static ImVec2 posstat = ImGui::GetWindowPos();
					ImVec2 pos = ImGui::GetWindowPos();
					BothXY(5, 7);
					ImGui::Checkbox("FakeLag", &Config::Misc->FakeLag); BothXY();
					ImGui::Checkbox("Adaptive", &Config::AntiAim->Adaptive); BothXY();
					ImGui::SliderInt("Amount:", &Config::Misc->ChokedPackets, 1, 14); BothXY(5, 7);

					ImGui::Checkbox("Slowwalk  ", &Config::AntiAim->Slowwalk); ImGui::SameLine();
					//ImGui::KeyButton("##slowwalkkey", &Config::AntiAim->SlowwalkKey); BothXY();
					ImGui::SliderInt("Units", &Config::AntiAim->SlowwalkSpeed, 45, 85); BothXY(5, 7);

					ImGui::Checkbox("FakeDuck  ", &Config::AntiAim->FakeDuck); ImGui::SameLine();
					//ImGui::KeyButton("##fakeduckkey", &Config::AntiAim->FakeDuckKey);

					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 385), ImVec2(pos.x + 320, pos.y + 390), ImColor(0, 112, 253));
					
				}ImGui::EndChild();
			}
			if (iTab == 3)
			{
				ImGui::Columns(4, nullptr, false);
				ImGui::SetColumnOffset(1, 10);
				ImGui::NextColumn();
				ImGui::Dummy(ImVec2(0, 20));
				ImGui::BeginChild("visuals_chams", ImVec2(320, 390), false, Flags);
				{
					static ImVec2 posstat = ImGui::GetWindowPos();
					ImVec2 pos = ImGui::GetWindowPos();

					//General
					BothXY(5, 7);
					ImGui::Checkbox("Enable ESP", &Config::ESP->Enabled);
					BothXY(5, 4);
					ImGui::Checkbox("Name", &Config::ESP->Name); BothXY();
					ImGui::Checkbox("Weapon", &Config::ESP->Weapon); BothXY();
					ImGui::Checkbox("Health", &Config::ESP->Health); BothXY();
					ImGui::Checkbox("Armor", &Config::ESP->Armor); BothXY();
					ImGui::Combo("Esp Target", &Config::ESP->Target, EspTargetList, ARRAYSIZE(EspTargetList)); BothXY();
					//ImGui::Checkbox("Aim Spot", &Config::ESP->AimSpot);

					ImGui::Combo("Chams", &Config::Render->ChamsTarget, ChamsTargetList, ARRAYSIZE(ChamsTargetList)); BothXY();
					ImGui::Combo("Mode", &Config::Render->ChamsMode[Config::Render->ChamsTarget], ChamsModeList, ARRAYSIZE(ChamsModeList)); BothXY();
					//ImGui::ColorPicker(Config::Colors->TestColor);
					ImGui::Checkbox("Outlined", &Config::Render->ChamsOutlined[Config::Render->ChamsTarget]);


					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 385), ImVec2(pos.x + 320, pos.y + 390), ImColor(0, 112, 253));
				}ImGui::EndChild();
			}
			if (iTab == 4) 
			{
				ImGui::Columns(4, nullptr, false);
				ImGui::SetColumnOffset(1, 10);
				ImGui::NextColumn();
				ImGui::Dummy(ImVec2(0, 20));
				ImGui::BeginChild("visuals_chams", ImVec2(320, 390), false, Flags);
				{
					static ImVec2 posstat = ImGui::GetWindowPos();
					ImVec2 pos = ImGui::GetWindowPos();

					//General
					BothXY(5, 7);
					ImGui::Checkbox("ThirdPerson ", &Config::Misc->ThirdPerson); ImGui::SameLine();
					//ImGui::KeyButton("##sdhhx2j", &Config::Misc->ThirdPersonKey); BothXY();
					if (Config::Misc->ThirdPerson)
					{
						ImGui::SliderFloat("X Rotate" ,&Config::Misc->ThirdXAdd, -180.f, 180.0f);BothXY();
						ImGui::SliderFloat("Y Rotate" ,&Config::Misc->ThirdYAdd, -180.f, 180.0f);BothXY();
						ImGui::SliderFloat("Distance" ,&Config::Misc->ThirdZAdd, 0.f, 200.0f);BothXY();
					}
					ImGui::Checkbox("No Smoke", &Config::Removals->NoSmoke); BothXY();
					ImGui::Checkbox("No Flash", &Config::Removals->NoFlash); BothXY();
					ImGui::Checkbox("Hitmarker", &Config::Misc->HitmarkerEnabled); BothXY();
					ImGui::Checkbox("BunnyHop", &Config::Misc->AutoJump); BothXY();
					ImGui::Checkbox("Auto Strafe", &Config::Misc->AutoStrafe);
					
					
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 385), ImVec2(pos.x + 320, pos.y + 390), ImColor(0, 112, 253));
				}
				ImGui::EndChild();
			}
			
		}
		ImGui::End();
		ImGui::Render();
	}

	bool Menu::OnKeyEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!Shared::m_bMenu)
			return false;

		
		if (ImGui_ImplDX9_WndProcHandler(hWnd, message, wParam, lParam))
			return true;

		return false;
	}

	void Menu::OnLostDevice()
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void Menu::OnResetDevice()
	{
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	void Menu::SetColors()
	{
		for (int i = 0; i < 4; i++)
		{

			m_flColors[LOCAL_COLOR][i] = Config::Colors->LOCAL_Chams[i] / 255.0f;

			m_flColors[TEAM_COLOR][i] = Config::Colors->TEAM_Chams_Normal[i] / 255.0f;
			m_flColors[TEAM_COLOR_WALL][i] = Config::Colors->TEAM_Chams_Colored[i] / 255.0f;
			m_flColors[TEAM_COLOR_OUTLINE][i] = Config::Colors->TEAM_Chams_Outline[i] / 255.0f;


			m_flColors[ENEMY_COLOR][i] = Config::Colors->ENEMY_Chams_Normal[i] / 255.0f;
			m_flColors[ENEMY_COLOR_WALL][i] = Config::Colors->ENEMY_Chams_Colored[i] / 255.0f;
			m_flColors[ENEMY_COLOR_OUTLINE][i] = Config::Colors->ENEMY_Chams_Outline[i] / 255.0f;

			m_flColors[WEAPON_COLOR][i] = Config::Colors->WEAPON_Chams_Normal[i] / 255.0f;
			m_flColors[WEAPON_COLOR_WALL][i] = Config::Colors->WEAPON_Chams_Colored[i] / 255.0f;
			m_flColors[WEAPON_COLOR_OUTLINE][i] = Config::Colors->WEAPON_Chams_Outline[i] / 255.0f;
		}
	}

	void Menu::ApplyColors()
	{
		for (int i = 0; i < 4; i++)
		{

			Config::Colors->LOCAL_Chams[i] = (int)(m_flColors[LOCAL_COLOR][i] * 255.0f);

			Config::Colors->TEAM_Chams_Normal[i] = (int)(m_flColors[TEAM_COLOR][i] * 255.0f);
			Config::Colors->TEAM_Chams_Colored[i] = (int)(m_flColors[TEAM_COLOR_WALL][i] * 255.0f);
			Config::Colors->TEAM_Chams_Outline[i] = (int)(m_flColors[TEAM_COLOR_OUTLINE][i] * 255.0f);


			Config::Colors->ENEMY_Chams_Normal[i] = (int)(m_flColors[ENEMY_COLOR][i] * 255.0f);
			Config::Colors->ENEMY_Chams_Colored[i] = (int)(m_flColors[ENEMY_COLOR_WALL][i] * 255.0f);
			Config::Colors->ENEMY_Chams_Outline[i] = (int)(m_flColors[ENEMY_COLOR_OUTLINE][i] * 255.0f);

			Config::Colors->WEAPON_Chams_Normal[i] = (int)(m_flColors[WEAPON_COLOR][i] * 255.0f);
			Config::Colors->WEAPON_Chams_Colored[i] = (int)(m_flColors[WEAPON_COLOR_WALL][i] * 255.0f);
			Config::Colors->WEAPON_Chams_Outline[i] = (int)(m_flColors[WEAPON_COLOR_OUTLINE][i] * 255.0f);

		}

		for (int i = 0; i < 3; i++)
		{
			ImGuiStyle& Style = ImGui::GetStyle();

		}
	}

}