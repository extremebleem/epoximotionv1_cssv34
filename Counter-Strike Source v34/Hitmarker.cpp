#include "HitMarker.hpp"
#include "Config.hpp"
#include "Source.hpp"
#include "Player.hpp"

float hurtTime = 0.f;

namespace Feature
{
	
	void HitMarker::FireGameEvent(IGameEvent* game_event)
	{
		if (game_event)
		{
			if (!std::strcmp(game_event->GetName(), "player_hurt"))
			{
				C_CSPlayer* local = C_CSPlayer::GetLocalPlayer();

				if (local)
				{
					int userid = game_event->GetInt("userid");
					int attacker = game_event->GetInt("attacker");
					int health = game_event->GetInt("health");
					int armor = game_event->GetInt("armor");
					int dmg = game_event->GetInt("dmg_health");
					if (local->GetIndex() == Source::m_pEngine->GetPlayerForUserID(attacker))
					{
						hurtTime = Source::m_pGlobalVars->curtime + 0.5f;
					}
				}
			}
		}
	}

	void HitMarker::Present()
	{
		if (Config::Misc->HitmarkerEnabled)
		{
			int W, H;
			static int AlphaDel = 0;
			Source::m_pEngine->GetScreenSize(W, H);
			
			if (hurtTime > Source::m_pGlobalVars->curtime)
			{
				Color white;
				white = Color(255, 255, 255, 255 - AlphaDel);
				
				if (int(Source::m_pGlobalVars->curtime) % 10 && AlphaDel < 240)
					AlphaDel += 15;

				Source::m_pRenderer->DrawLine(W / 2 - 10, H / 2 - 10, W / 2 - 5, H / 2 - 5, white);
				Source::m_pRenderer->DrawLine(W / 2 - 10, H / 2 + 10, W / 2 - 5, H / 2 + 5, white);
				Source::m_pRenderer->DrawLine(W / 2 + 10, H / 2 - 10, W / 2 + 5, H / 2 - 5, white);
				Source::m_pRenderer->DrawLine(W / 2 + 10, H / 2 + 10, W / 2 + 5, H / 2 + 5, white);
			}
			else
			{
				AlphaDel = 0;
			}
		}
	}
}