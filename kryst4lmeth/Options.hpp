#pragma once
#include <Windows.h>
#include <string>
#include <fstream>
#include "XorStr.hpp"

struct Options
{
	//Here we defined the extern variables declared on Options.hpp
	int g_iMenuPage = 0;

	bool g_bMainWindowOpen = true;

	bool g_bESPShowBoxes = true;
	bool g_bESPShowSkeletons = true;
	bool g_bESPShowPlayerNames = true;
	bool g_bESPShowWeaponNames = true;
	bool g_bBunnyhop = true;
	bool g_bAutoStrafe = false;

	bool g_bEnableUntrusted = false;
	bool g_bTriggerbot = false;
	bool g_bAutoShoot = false;
	bool g_bAutoWall = false;
	bool g_bResolve = false;
	bool g_bAirStuck = false;
	bool g_bCircleStrafe = false;
	bool g_bNoVisualRecoil = false;

	float g_fAimbotFOV = 3.0f;
	float g_fAimbotSpeed = 2.5f;

	int g_iTagOption = 7;
	char g_cCustomTag[260] = {};

	int g_iFOV = 90;

	bool g_bHvHAARandomize = false;

	int g_iESPQuality = 2;
	int g_iPlayerChams = 1;
	int g_iViewmodelChams = 1;
	int g_iWeaponChams = 1;
	int g_iRCSAccuracy = 0;
	int g_iAimAssistMode = 0;
	int g_iAimbotTarget = 0;
	int g_iAutoShootTarget = 0;
	int g_iViewMode = 1;

	int g_iAntiAim_Pitch = 0;
	int g_iAntiAim_Yaw = 0;
	int g_iAntiAim_Direction = 0;

	bool g_bAntiAim_OverrideAngles = false;

	int g_iCustomAntiAim_PitchOverride = 0;
	int g_iCustomAntiAim_RealYawOverride = 0;
	int g_iCustomAntiAim_RealDirectionOverride = 0;

	bool g_bCustomAntiAim_ApplyFakeAngle = false;

	int g_iCustomAntiAim_FakeYawOverride = 0;
	int g_iCustomAntiAim_FakeDirectionOverride = 0;

	int g_iAutoWall_MinDamage = 1;
	int g_iHitScan_Resolution = 50;
	int g_iFakeLag_Mode = 0;
	int g_iFakeLag_LagFactor = 0;
	int g_iAutoShootMode = 0;
	int g_iFakeLagFix = 0;
	int g_iMinHitChance = 0;

	bool g_bLowerBodyYawFix = false;
	int g_iResolver_PitchMode = 0;
	int g_iResolver_YawMode = 1;
	int g_iResolver_AngleStep = 1;
	int g_iResolver_AngleSnapping = 0;

	float g_iScanWeights[8] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	int g_iChams_TeamColor[3] = { 0, 255, 255 };
	float g_fChams_Alpha = 0.8f;

	float g_fASUSOpacity = 1.0f;

	bool g_bVisualizeHitscan = false;
	bool g_bAutoResetScore = false;
	bool g_bAutoNameSteal = false;
};

extern int ConfigSlot;
extern Options Config;

static wchar_t* CharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

static void LoadConfig()
{
	CreateDirectory(CharArrayToLPCWSTR(XorStr("configs")), NULL);
	std::string ConfigName;
	ConfigName = XorStr("configs/");
	ConfigName += XorStr("config");
	ConfigName += std::to_string(ConfigSlot).c_str();
	ConfigName += XorStr(".dat");

	std::ifstream f(ConfigName.c_str());
	if (!f.good()) return;

	FILE* fin = fopen(ConfigName.c_str(), XorStr("r"));
	fread(&Config, sizeof(Options), 1, fin);
	fclose(fin);
}

static void SaveConfig()
{
	CreateDirectory(CharArrayToLPCWSTR(XorStr("configs")), NULL);
	std::string ConfigName;
	ConfigName = XorStr("configs/");
	ConfigName += XorStr("config");
	ConfigName += std::to_string(ConfigSlot).c_str();
	ConfigName += XorStr(".dat");
	FILE* fout = fopen(ConfigName.c_str(), XorStr("w"));
	fwrite(&Config, sizeof(Options), 1, fout);
	fclose(fout);
}