#include "../SDK/PluginSDK.h"
#include "../SDK/EventArgs.h"
#include "../SDK/EventHandler.h"
#include <limits>

PLUGIN_API const char PLUGIN_PRINT_NAME[32] = "Kled";
PLUGIN_API const char PLUGIN_PRINT_AUTHOR[32] = "BiggieSmalls";
PLUGIN_API ChampionId PLUGIN_TARGET_CHAMP = ChampionId::Kled;

namespace Menu
{
	IMenu* MenuInstance = nullptr;
	//Hey
	namespace Combo
	{
		IMenuElement* Enabled = nullptr;
		IMenuElement* UseQ = nullptr;
		IMenuElement* UseQ1 = nullptr;
		IMenuElement* UseE = nullptr;
		IMenuElement* UseE1 = nullptr;
		IMenuElement* TowerHarass = nullptr;
	}

	namespace Harass
	{
		IMenuElement* Enabled = nullptr;
		IMenuElement* UseQ = nullptr;
		IMenuElement* UseQ1 = nullptr;
		IMenuElement* UseE = nullptr;
		IMenuElement* UseE1 = nullptr;
		IMenuElement* TowerHarass = nullptr;
	}
	namespace LaneClear
	{
		IMenuElement* Enabled = nullptr;
		IMenuElement* UseQ = nullptr;
		IMenuElement* UseE = nullptr;
		IMenuElement* MinMinions = nullptr;
	}
	namespace Killsteal
	{
		IMenuElement* Enabled = nullptr;
		IMenuElement* UseQ = nullptr;
	}

	namespace Misc
	{
		IMenuElement* Qdash = nullptr;
		IMenuElement* QFlee = nullptr;
		IMenuElement* Eflee = nullptr;
		IMenuElement* Qsemi = nullptr;
	}

	namespace Drawings
	{
		IMenuElement* Toggle = nullptr;
		IMenuElement* DrawQRange = nullptr;
		IMenuElement* DrawQ1Range = nullptr;
		IMenuElement* DrawERange = nullptr;
		IMenuElement* DrawRRange = nullptr;
	}

	namespace Colors
	{
		IMenuElement* QColor = nullptr;
		IMenuElement* Q1Color = nullptr;
		IMenuElement* EColor = nullptr;
		IMenuElement* RColor = nullptr;
	}
}

namespace Spells
{
	std::shared_ptr<ISpell> Q = nullptr;
	std::shared_ptr<ISpell> Q1 = nullptr;
	std::shared_ptr<ISpell> W = nullptr;
	std::shared_ptr<ISpell> E = nullptr;
	std::shared_ptr<ISpell> E1 = nullptr;
	std::shared_ptr<ISpell> R = nullptr;
}

//number of  enemies in Ult Radius
int CountEnemiesInRangeR(const Vector& Position, const float Range)
{
	auto Enemies = g_ObjectManager->GetChampions(false);
	int Counter = 0;
	for (auto& Enemy : Enemies)
		if (Enemy->IsVisible() && Enemy->IsValidTarget() && Position.Distance(Enemy->Position()) <= Range)
			Counter++;
}

//r we mounted
bool IsKled()
{
	auto spellbook = g_LocalPlayer->GetSpellbook();
	auto KledQ = spellbook->GetSpell(SpellSlot::Q);
	auto KledQData = KledQ->SData();
	if (KledQData.SpellName == "KledQ")
		return true;
	else
		return false;
}

// Combo Logic
void ComboLogic()
{
	if (Menu::Combo::TowerHarass->GetBool() && g_LocalPlayer->IsUnderMyEnemyTurret())
		return;

	if (Menu::Combo::Enabled->GetBool())
	{
		auto TargetE = g_Common->GetTarget(Spells::E->Range(), DamageType::Physical);

		// Use first E will auto recast before E expires only use if Q or W ready
		if (IsKled() && Menu::Combo::UseE->GetBool() && Spells::E->IsReady())
			if (TargetE && TargetE->IsValidTarget() && TargetE->IsInRange(550.f) && !g_LocalPlayer->HasBuff("KledE2") && (Spells::W->IsReady() || Spells::Q->IsReady()))
				Spells::E->Cast(TargetE, HitChance::High);

		// Use second E before it expires
		if (IsKled() && Menu::Combo::UseE1->GetBool() && Spells::E->IsReady())
			if (TargetE && TargetE->IsValidTarget() && TargetE->HasBuff("klede2target") && g_LocalPlayer->BuffTimeLeft(hash("KledE2")) <= .5f)
				Spells::E->Cast(TargetE);

		//Use Q Bear Trap

		{
			if (Menu::Combo::UseQ->GetBool() && Spells::Q->IsReady() && IsKled())
			{
				auto TargetQ = g_Common->GetTarget(Spells::Q->Range(), DamageType::Physical);

				if (TargetQ && TargetQ->IsValidTarget() && g_LocalPlayer->Distance(TargetQ) <= 770)
					Spells::Q->Cast(TargetQ, HitChance::High);
			}
		}
		{
			//Use Q Shotgun
			if (Menu::Combo::UseQ1->GetBool() && Spells::Q1->IsReady() && !IsKled())
			{
				auto TargetQ1 = g_Common->GetTarget(Spells::Q1->Range(), DamageType::Physical);

				if (TargetQ1 && TargetQ1->IsValidTarget() && g_LocalPlayer->Distance(TargetQ1) <= 650)
					Spells::Q->Cast(TargetQ1, HitChance::High);
			}
		}
	}
}

// Harass Logic
void HarassLogic()
{
	if (Menu::Harass::TowerHarass->GetBool() && g_LocalPlayer->IsUnderMyEnemyTurret())
		return;

	if (Menu::Harass::Enabled->GetBool())
	{
		auto TargetE = g_Common->GetTarget(Spells::E->Range(), DamageType::Physical);

		// Use first E will auto recast before E expires only use if Q or W ready
		if (IsKled() && Menu::Harass::UseE->GetBool() && Spells::E->IsReady())
			if (TargetE && TargetE->IsValidTarget() && TargetE->IsInRange(550.f) && (Spells::W->IsReady() || TargetE->HasBuff("kledqmark")) && !g_LocalPlayer->HasBuff("KledE2"))
				Spells::E->Cast(TargetE, HitChance::High);
		{
			// Use second E before it expires
			if (IsKled() && Menu::Harass::UseE1->GetBool() && Spells::E->IsReady())
				if (TargetE && TargetE->IsValidTarget() && TargetE->HasBuff("klede2target") && g_LocalPlayer->BuffTimeLeft(hash("KledE2")) <= .5f)
					Spells::E->Cast(TargetE);
		}
		//Use Q Bear Trap

		{
			if (Menu::Harass::UseQ->GetBool() && Spells::Q->IsReady() && IsKled())
			{
				auto TargetQ = g_Common->GetTarget(Spells::Q->Range(), DamageType::Physical);

				if (TargetQ && TargetQ->IsValidTarget() && g_LocalPlayer->Distance(TargetQ) <= 770)
					Spells::Q->Cast(TargetQ, HitChance::High);
			}
		}
		{
			//Use Q Shotgun
			if (Menu::Harass::UseQ1->GetBool() && Spells::Q1->IsReady() && !IsKled())
			{
				auto TargetQ1 = g_Common->GetTarget(Spells::Q1->Range(), DamageType::Physical);

				if (TargetQ1 && TargetQ1->IsValidTarget() && g_LocalPlayer->Distance(TargetQ1) <= 650)
					Spells::Q->Cast(TargetQ1, HitChance::High);
			}
		}
	}
}

// killsteal
void KillstealLogic()
{
	const auto Enemies = g_ObjectManager->GetChampions(false);
	for (auto Enemy : Enemies)
	{
		if (IsKled() && Menu::Killsteal::UseQ->GetBool() && Spells::Q->IsReady() && Enemy->IsInRange(Spells::Q->Range()))
		{
			auto QDamage = g_Common->GetSpellDamage(g_LocalPlayer, Enemy, SpellSlot::Q, false);
			if (Enemy->IsValidTarget() && QDamage >= Enemy->RealHealth(true, false))
				Spells::Q->Cast(Enemy);
		}
		if (!IsKled() && Menu::Killsteal::UseQ->GetBool() && Spells::Q1->IsReady() && Enemy->IsInRange(Spells::Q1->Range()))
		{
			auto QDamage = g_Common->GetSpellDamage(g_LocalPlayer, Enemy, SpellSlot::Q, false);
			if (Enemy->IsValidTarget() && QDamage >= Enemy->RealHealth(true, false))
				Spells::Q1->Cast(Enemy);
		}
	}
}

// Lane Clear Logic
void LaneCLearLogic()
{
	auto MinMinions = Menu::LaneClear::MinMinions->GetInt();
	if (!MinMinions)
		return;

	auto Target = g_Orbwalker->GetTarget();
	{
		if (IsKled() && Target && (Target->IsMinion() || Target->IsMonster()) && Spells::Q->Range());

		if (Spells::Q->IsReady() && Menu::LaneClear::UseQ->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear))
			Spells::Q->CastOnBestFarmPosition(MinMinions);
	}

	// Jungle Clear Logic
	{
		auto Monster = g_Orbwalker->GetTarget();
		{
			if (IsKled() && Monster && Monster->IsMonster() && Menu::LaneClear::UseQ->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) && Spells::Q->IsReady())
				Spells::Q->Cast(Monster, HitChance::High);
		}
	}
}
// Flee Mode Logic
void FleeLogic()
{
	if (Menu::Misc::Eflee->GetBool() && Spells::E->IsReady() && IsKled() && !g_LocalPlayer->HasBuff("KledE2"))
		Spells::E->Cast(g_Common->CursorPosition());

	// Flee knockback opposite direction
	if (Menu::Misc::QFlee->GetBool() && Spells::Q1->IsReady() && !IsKled())
		Spells::Q1->Cast(g_Common->CursorPosition().Extend(g_LocalPlayer->Position(), g_LocalPlayer->Distance(g_Common->CursorPosition()) + 100));
}

//Semi-Manual Q
void QsemiLogic()
{
	if (Menu::Misc::Qsemi->GetBool() && !IsKled())
	{
		auto Target = g_Common->GetTarget(Spells::Q1->Range(), DamageType::Physical);
		if (Target && Target->IsValidTarget())
			Spells::Q1->Cast(Target, HitChance::High);
	}
}

void OnGameUpdate()
{
	// adjusting R range as you level them up
	if (Spells::R->Level() == 2)
		Spells::R->SetRange(4000);
	if (Spells::R->Level() == 3)
		Spells::R->SetRange(4500);

	if (g_LocalPlayer->IsDead())
		return;

	if (Menu::Combo::Enabled->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo))
		ComboLogic();

	if (Menu::Harass::Enabled->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeHarass))
		HarassLogic();

	if (Menu::LaneClear::Enabled->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear))
		LaneCLearLogic();

	if (Menu::Killsteal::Enabled->GetBool())
		KillstealLogic();

	if (Menu::Misc::Eflee->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeFlee))
		FleeLogic();

	if (Menu::Misc::Qsemi->GetBool())
	{
		g_Orbwalker->MoveTo(g_Common->CursorPosition());
		QsemiLogic();
	}

	// E on Dash
	if (Menu::Misc::Qdash->GetBool() && Spells::Q->IsReady() && IsKled() && !g_LocalPlayer->HasBuff("KledE2"))
	{
		const auto Enemies = g_ObjectManager->GetChampions(false);
		for (auto Enemy : Enemies)
		{
			if (Enemy->IsDashing())
			{
				const auto DashData = Enemy->GetDashData();
				if (g_LocalPlayer->Distance(DashData.EndPosition) < 630.f)
					Spells::Q->Cast(Enemy, HitChance::High);
			}
		}
	}
}

// Drawings
void OnHudDraw()
{
	if (!Menu::Drawings::Toggle->GetBool() || g_LocalPlayer->IsDead())
		return;

	const auto PlayerPosition = g_LocalPlayer->Position();
	const auto CirclesWidth = 1.5f;

	if (Menu::Drawings::DrawQRange->GetBool() && !Spells::Q->CooldownTime() && IsKled())
		g_Drawing->AddCircle(PlayerPosition, Spells::Q->Range(), Menu::Colors::QColor->GetColor(), CirclesWidth);

	if (Menu::Drawings::DrawQ1Range->GetBool() && !Spells::Q1->CooldownTime() && !IsKled())
		g_Drawing->AddCircle(PlayerPosition, Spells::Q1->Range(), Menu::Colors::Q1Color->GetColor(), CirclesWidth);

	if (Menu::Drawings::DrawERange->GetBool() && !Spells::E->CooldownTime())
		g_Drawing->AddCircle(PlayerPosition, Spells::E->Range(), Menu::Colors::EColor->GetColor(), CirclesWidth);

	if (Menu::Drawings::DrawRRange->GetBool() && !Spells::R->CooldownTime())
		g_Drawing->AddCircle(PlayerPosition, Spells::R->Range(), Menu::Colors::RColor->GetColor(), CirclesWidth);
}

PLUGIN_API bool OnLoadSDK(IPluginsSDK* plugin_sdk)
{
	DECLARE_GLOBALS(plugin_sdk);

	if (g_LocalPlayer->ChampionId() != ChampionId::Kled)
		return false;

	using namespace Menu;
	using namespace Spells;

	MenuInstance = g_Menu->CreateMenu("Kled", "Kled by BiggieSmalls");

	const auto ComboSubMenu = MenuInstance->AddSubMenu("Combo", "combo_menu");
	Menu::Combo::Enabled = ComboSubMenu->AddCheckBox("Enable Combo", "enable_combo", true);
	Menu::Combo::UseQ = ComboSubMenu->AddCheckBox("Q Bear Trap", "combo_use_q", true);
	Menu::Combo::UseQ1 = ComboSubMenu->AddCheckBox("Q ShotGun", "combo_use_q1", false);
	Menu::Combo::UseE = ComboSubMenu->AddCheckBox("Use E", "combo_use_e", true);
	Menu::Combo::UseE1 = ComboSubMenu->AddCheckBox("Use Dash Back E", "combo_use_e1", true);
	Menu::Combo::TowerHarass = ComboSubMenu->AddCheckBox("Undertower Harass", "tower_harass_combo", true);

	const auto HarassSubMenu = MenuInstance->AddSubMenu("Harass", "harass_menu");
	Menu::Harass::Enabled = HarassSubMenu->AddCheckBox("Enable Harass", "enable_harass", true);
	Menu::Harass::UseQ = HarassSubMenu->AddCheckBox("Use Bear Trap", "harass_use_q", false);
	Menu::Harass::UseQ1 = HarassSubMenu->AddCheckBox("Use Shotgun", "harass_use_w", false);
	Menu::Harass::UseE = HarassSubMenu->AddCheckBox("Use E", "harass_use_e", false);
	Menu::Harass::UseE1 = HarassSubMenu->AddCheckBox("Use Dash Back E", "harass_use_e1", false);
	Menu::Harass::TowerHarass = HarassSubMenu->AddCheckBox("Undertower Harass", "tower_harass", true);

	const auto LaneClearSubMenu = MenuInstance->AddSubMenu("Lane Clear", "laneclear_menu");
	Menu::LaneClear::Enabled = LaneClearSubMenu->AddCheckBox("Enable Lane Clear", "enable_laneclear", true);
	Menu::LaneClear::UseQ = LaneClearSubMenu->AddCheckBox("Use Q", "laneclear_use_q", false);
	Menu::LaneClear::MinMinions = LaneClearSubMenu->AddSlider("Min minions to use Q", "lane_clear_min_minions", 4, 0, 9);

	const auto KSSubMenu = MenuInstance->AddSubMenu("KS", "ks_menu");
	Menu::Killsteal::Enabled = KSSubMenu->AddCheckBox("Enable Killsteal", "enable_ks", true);
	Menu::Killsteal::UseQ = KSSubMenu->AddCheckBox("Use Q", "q_ks", true);

	const auto MiscSubMenu = MenuInstance->AddSubMenu("Misc", "misc_menu");
	Menu::Misc::Qdash = MiscSubMenu->AddCheckBox("Auto Q on dashing", "q_dashing_enemy", true);
	Menu::Misc::QFlee = MiscSubMenu->AddCheckBox("Use Shotgun in Flee Mode", "q_flee", true);
	Menu::Misc::Eflee = MiscSubMenu->AddCheckBox("Use E in Flee Mode", "e_flee", true);
	Menu::Misc::Qsemi = MiscSubMenu->AddKeybind("Semi Manual Shotgun Q", "semi_manual_q", 'T', false, KeybindType_Hold);

	const auto DrawingsSubMenu = MenuInstance->AddSubMenu("Drawings", "drawings_menu");
	Drawings::Toggle = DrawingsSubMenu->AddCheckBox("Enable Drawings", "drawings_toggle", true);
	Drawings::DrawQRange = DrawingsSubMenu->AddCheckBox("Draw Q Range", "draw_q", true);
	Drawings::DrawQ1Range = DrawingsSubMenu->AddCheckBox("Draw Shotgun Range", "draw_q1", true);
	Drawings::DrawRRange = DrawingsSubMenu->AddCheckBox("Draw R Range", "draw_r", true);
	Drawings::DrawERange = DrawingsSubMenu->AddCheckBox("Draw E Range", "draw_e", true);

	const auto ColorsSubMenu = DrawingsSubMenu->AddSubMenu("Colors", "color_menu");
	Colors::QColor = ColorsSubMenu->AddColorPicker("Bear Trap Range", "color_q_range", 23, 175, 255, 180);
	Colors::Q1Color = ColorsSubMenu->AddColorPicker("Shotgun Range", "color_q1_range", 23, 275, 215, 180);
	Colors::RColor = ColorsSubMenu->AddColorPicker("R Range", "color_r_range", 230, 120, 198, 180);
	Colors::EColor = ColorsSubMenu->AddColorPicker("E Range", "color_e_range", 210, 210, 210, 180);

	Spells::Q = g_Common->AddSpell(SpellSlot::Q, 710.f); //bear trap
	Spells::Q1 = g_Common->AddSpell(SpellSlot::Q, 630.f); //shotgun
	Spells::W = g_Common->AddSpell(SpellSlot::W);
	Spells::E = g_Common->AddSpell(SpellSlot::E, 530.f);
	Spells::R = g_Common->AddSpell(SpellSlot::R, 3500.f);

	//prediction and customs
	Spells::Q->SetSkillshot(0.25f, 40.f, 1300.f, kCollidesWithNothing && kCollidesWithYasuoWall, kSkillshotLine);       //  Bear Trap
	Spells::Q1->SetSkillshot(0.25f, 40.f, 2000.f, kCollidesWithMinions && kCollidesWithYasuoWall, kSkillshotLine);   //  Shotgun
	Spells::E->SetSkillshot(0.0f, 100.f, 945.f, kCollidesWithNothing, kSkillshotLine);

	//EventHandler<Events::OnProcessSpellCast>::AddEventHandler(OnProcessSpell);
	//EventHandler<Events::OnBuff>::AddEventHandler(OnBuffChange);
	EventHandler<Events::GameUpdate>::AddEventHandler(OnGameUpdate);
	//EventHandler<Events::OnAfterAttackOrbwalker>::AddEventHandler(OnAfterAttack);
	EventHandler<Events::OnHudDraw>::AddEventHandler(OnHudDraw);

	//EventHandler<Events::OnBeforeAttackOrbwalker>::AddEventHandler(OnBeforeAttack);

	g_Common->ChatPrint("<font color='#FFC300'>Kled Loaded!</font>");
	g_Common->Log("Kled plugin loaded.");

	return true;
}

PLUGIN_API void OnUnloadSDK()
{
	Menu::MenuInstance->Remove();

	//EventHandler<Events::OnBeforeAttackOrbwalker>::RemoveEventHandler(OnBeforeAttack);
	EventHandler<Events::GameUpdate>::RemoveEventHandler(OnGameUpdate);
	//EventHandler<Events::OnProcessSpellCast>::RemoveEventHandler(OnProcessSpell);
	EventHandler<Events::GameUpdate>::RemoveEventHandler(OnGameUpdate);
	//EventHandler<Events::OnAfterAttackOrbwalker>::RemoveEventHandler(OnAfterAttack);
	EventHandler<Events::OnHudDraw>::RemoveEventHandler(OnHudDraw);
	//EventHandler<Events::OnBuff>::RemoveEventHandler(OnBuffChange);

	g_Common->ChatPrint("<font color='#00BFFF'>Kled Unloaded.</font>");
}