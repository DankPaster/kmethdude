#include "EntityESP.hpp"
#include "Options.hpp"
#include "Utils.hpp"

#include "AutoWall.h"
#include "AutoShoot.h"
#include <math.h>
#include "StudioHDR.h"

using namespace DrawTools;
using namespace SourceEngine;

void EntityESP::RenderESP()
{
    using namespace SourceEngine;

    auto pLocal = C_CSPlayer::GetLocalPlayer();

    if (!pLocal) return;

    //Checks if its an enemy
    auto bIsEnemy = pLocal->GetTeamNum() != m_pEntity->GetTeamNum();

	int EnemyHealth = m_pEntity->GetHealth();
	float HealthFrac = EnemyHealth / 100.0f;

	Vector4D EnemyColor;
	Vector4D EnemyColorBG;
	if (HealthFrac > 0.5f)
	{
		EnemyColor = Vector4D(255.0f * (1 - HealthFrac) * 2.0f, 255.0f, 0, 255.0f);
		EnemyColorBG = EnemyColor / 3.0f;
	}
	else
	{
		EnemyColor = Vector4D(255.0f, 255.0f * HealthFrac * 2.0f, 0, 255.0f);
		EnemyColorBG = EnemyColor / 3.0f;
	}

	Vector4D AllyColor(Config.g_iChams_TeamColor[0], Config.g_iChams_TeamColor[1], Config.g_iChams_TeamColor[2], 255);
	Vector4D AllyColorBG = AllyColor / 3.0f;

    //Get the appropriate drawing color
    auto SourceColor = bIsEnemy ?
		Color(int(EnemyColor.x), int(EnemyColor.y), int(EnemyColor.z), int(EnemyColor.w)) :
		Color(int(AllyColor.x), int(AllyColor.y), int(AllyColor.z), int(AllyColor.w));
	auto SourceColorBG = bIsEnemy ?
		Color(int(EnemyColorBG.x), int(EnemyColorBG.y), int(EnemyColorBG.z), 255) :
		Color(int(AllyColorBG.x), int(AllyColorBG.y), int(AllyColorBG.z), 255);

	auto vOrigin = m_pEntity->GetOrigin();
    auto vHead = m_pEntity->GetBoneByName(XorStr("head_0")) + Vector(0, 0, 8);

    float height = std::fabs(vHead.z - vOrigin.z);
    float width = height * 0.5f;
	float width_diag = width / 2.4f;
	float yaw_rad = m_pEntity->GetEyeAngles().y * M_PI / 180.0f;

	Vector TopVertices[4];
	Vector BottomVertices[4];
	Vector IntermediateVertices[4];

	Vector p = vOrigin;

	Utils::WorldToScreen(Vector(p.x + width_diag * cos(yaw_rad) - width_diag * sin(yaw_rad), p.y + width_diag * cos(yaw_rad) + width_diag * sin(yaw_rad), p.z + height), TopVertices[0]);
	Utils::WorldToScreen(Vector(p.x + width_diag * cos(yaw_rad) + width_diag * sin(yaw_rad), p.y + -width_diag * cos(yaw_rad) + width_diag * sin(yaw_rad), p.z + height), TopVertices[1]);
	Utils::WorldToScreen(Vector(p.x + -width_diag * cos(yaw_rad) + width_diag * sin(yaw_rad), p.y + -width_diag * cos(yaw_rad) - width_diag * sin(yaw_rad), p.z + height), TopVertices[2]);
	Utils::WorldToScreen(Vector(p.x + -width_diag * cos(yaw_rad) - width_diag * sin(yaw_rad), p.y + width_diag * cos(yaw_rad) - width_diag * sin(yaw_rad), p.z + height), TopVertices[3]);
	Utils::WorldToScreen(Vector(p.x + width_diag * cos(yaw_rad) - width_diag * sin(yaw_rad), p.y + width_diag * cos(yaw_rad) + width_diag * sin(yaw_rad), p.z), BottomVertices[0]);
	Utils::WorldToScreen(Vector(p.x + width_diag * cos(yaw_rad) + width_diag * sin(yaw_rad), p.y + -width_diag * cos(yaw_rad) + width_diag * sin(yaw_rad), p.z), BottomVertices[1]);
	Utils::WorldToScreen(Vector(p.x + -width_diag * cos(yaw_rad) + width_diag * sin(yaw_rad), p.y + -width_diag * cos(yaw_rad) - width_diag * sin(yaw_rad), p.z), BottomVertices[2]);
	Utils::WorldToScreen(Vector(p.x + -width_diag * cos(yaw_rad) - width_diag * sin(yaw_rad), p.y + width_diag * cos(yaw_rad) - width_diag * sin(yaw_rad), p.z), BottomVertices[3]);
	IntermediateVertices[0] = (TopVertices[0] * HealthFrac) + (BottomVertices[0] * (1.0f - HealthFrac));
	IntermediateVertices[1] = (TopVertices[1] * HealthFrac) + (BottomVertices[1] * (1.0f - HealthFrac));
	IntermediateVertices[2] = (TopVertices[2] * HealthFrac) + (BottomVertices[2] * (1.0f - HealthFrac));
	IntermediateVertices[3] = (TopVertices[3] * HealthFrac) + (BottomVertices[3] * (1.0f - HealthFrac));

	auto SourceColorTransparent = bIsEnemy ?
		Color(int(EnemyColor.x), int(EnemyColor.y), int(EnemyColor.z), int(13)) :
		Color(int(AllyColor.x), int(AllyColor.y), int(AllyColor.z), int(13));
	auto SourceColorBGTransparent = bIsEnemy ?
		Color(int(EnemyColorBG.x), int(EnemyColorBG.y), int(EnemyColorBG.z), 30) :
		Color(int(AllyColorBG.x), int(AllyColorBG.y), int(AllyColorBG.z), 30);

	if (EnemyHealth != 100)
	{
		DrawFilledQuad(Vector2D(TopVertices[0].x, TopVertices[0].y), Vector2D(TopVertices[1].x, TopVertices[1].y), Vector2D(IntermediateVertices[1].x, IntermediateVertices[1].y), Vector2D(IntermediateVertices[0].x, IntermediateVertices[0].y), SourceColorBGTransparent);
		DrawFilledQuad(Vector2D(TopVertices[1].x, TopVertices[1].y), Vector2D(TopVertices[2].x, TopVertices[2].y), Vector2D(IntermediateVertices[2].x, IntermediateVertices[2].y), Vector2D(IntermediateVertices[1].x, IntermediateVertices[1].y), SourceColorBGTransparent);
		DrawFilledQuad(Vector2D(TopVertices[2].x, TopVertices[2].y), Vector2D(TopVertices[3].x, TopVertices[3].y), Vector2D(IntermediateVertices[3].x, IntermediateVertices[3].y), Vector2D(IntermediateVertices[2].x, IntermediateVertices[2].y), SourceColorBGTransparent);
		DrawFilledQuad(Vector2D(TopVertices[3].x, TopVertices[3].y), Vector2D(TopVertices[0].x, TopVertices[0].y), Vector2D(IntermediateVertices[0].x, IntermediateVertices[0].y), Vector2D(IntermediateVertices[3].x, IntermediateVertices[3].y), SourceColorBGTransparent);

		DrawFilledQuad(Vector2D(IntermediateVertices[0].x, IntermediateVertices[0].y), Vector2D(IntermediateVertices[1].x, IntermediateVertices[1].y), Vector2D(BottomVertices[1].x, BottomVertices[1].y), Vector2D(BottomVertices[0].x, BottomVertices[0].y), SourceColorTransparent);
		DrawFilledQuad(Vector2D(IntermediateVertices[1].x, IntermediateVertices[1].y), Vector2D(IntermediateVertices[2].x, IntermediateVertices[2].y), Vector2D(BottomVertices[2].x, BottomVertices[2].y), Vector2D(BottomVertices[1].x, BottomVertices[1].y), SourceColorTransparent);
		DrawFilledQuad(Vector2D(IntermediateVertices[2].x, IntermediateVertices[2].y), Vector2D(IntermediateVertices[3].x, IntermediateVertices[3].y), Vector2D(BottomVertices[3].x, BottomVertices[3].y), Vector2D(BottomVertices[2].x, BottomVertices[2].y), SourceColorTransparent);
		DrawFilledQuad(Vector2D(IntermediateVertices[3].x, IntermediateVertices[3].y), Vector2D(IntermediateVertices[0].x, IntermediateVertices[0].y), Vector2D(BottomVertices[0].x, BottomVertices[0].y), Vector2D(BottomVertices[3].x, BottomVertices[3].y), SourceColorTransparent);

		DrawFilledQuad(Vector2D(TopVertices[0].x, TopVertices[0].y), Vector2D(TopVertices[1].x, TopVertices[1].y), Vector2D(TopVertices[2].x, TopVertices[2].y), Vector2D(TopVertices[3].x, TopVertices[3].y), SourceColorBGTransparent);
		DrawFilledQuad(Vector2D(BottomVertices[0].x, BottomVertices[0].y), Vector2D(BottomVertices[1].x, BottomVertices[1].y), Vector2D(BottomVertices[2].x, BottomVertices[2].y), Vector2D(BottomVertices[3].x, BottomVertices[3].y), SourceColorTransparent);
	}
	else
	{
		DrawFilledQuad(Vector2D(TopVertices[0].x, TopVertices[0].y), Vector2D(TopVertices[1].x, TopVertices[1].y), Vector2D(BottomVertices[1].x, BottomVertices[1].y), Vector2D(BottomVertices[0].x, BottomVertices[0].y), SourceColorTransparent);
		DrawFilledQuad(Vector2D(TopVertices[1].x, TopVertices[1].y), Vector2D(TopVertices[2].x, TopVertices[2].y), Vector2D(BottomVertices[2].x, BottomVertices[2].y), Vector2D(BottomVertices[1].x, BottomVertices[1].y), SourceColorTransparent);
		DrawFilledQuad(Vector2D(TopVertices[2].x, TopVertices[2].y), Vector2D(TopVertices[3].x, TopVertices[3].y), Vector2D(BottomVertices[3].x, BottomVertices[3].y), Vector2D(BottomVertices[2].x, BottomVertices[2].y), SourceColorTransparent);
		DrawFilledQuad(Vector2D(TopVertices[3].x, TopVertices[3].y), Vector2D(TopVertices[0].x, TopVertices[0].y), Vector2D(BottomVertices[0].x, BottomVertices[0].y), Vector2D(BottomVertices[3].x, BottomVertices[3].y), SourceColorTransparent);

		DrawFilledQuad(Vector2D(TopVertices[0].x, TopVertices[0].y), Vector2D(TopVertices[1].x, TopVertices[1].y), Vector2D(TopVertices[2].x, TopVertices[2].y), Vector2D(TopVertices[3].x, TopVertices[3].y), SourceColorTransparent);
		DrawFilledQuad(Vector2D(BottomVertices[0].x, BottomVertices[0].y), Vector2D(BottomVertices[1].x, BottomVertices[1].y), Vector2D(BottomVertices[2].x, BottomVertices[2].y), Vector2D(BottomVertices[3].x, BottomVertices[3].y), SourceColorTransparent);
	}

	std::vector<Line> BatchLines;

	if (EnemyHealth != 100)
	{
		BatchLines.push_back(Line(SourceColorBG, TopVertices[0].x, TopVertices[0].y, TopVertices[1].x, TopVertices[1].y));
		BatchLines.push_back(Line(SourceColorBG, TopVertices[1].x, TopVertices[1].y, TopVertices[2].x, TopVertices[2].y));
		BatchLines.push_back(Line(SourceColorBG, TopVertices[2].x, TopVertices[2].y, TopVertices[3].x, TopVertices[3].y));
		BatchLines.push_back(Line(SourceColorBG, TopVertices[3].x, TopVertices[3].y, TopVertices[0].x, TopVertices[0].y));
	}
	else
	{
		BatchLines.push_back(Line(SourceColor, TopVertices[0].x, TopVertices[0].y, TopVertices[1].x, TopVertices[1].y));
		BatchLines.push_back(Line(SourceColor, TopVertices[1].x, TopVertices[1].y, TopVertices[2].x, TopVertices[2].y));
		BatchLines.push_back(Line(SourceColor, TopVertices[2].x, TopVertices[2].y, TopVertices[3].x, TopVertices[3].y));
		BatchLines.push_back(Line(SourceColor, TopVertices[3].x, TopVertices[3].y, TopVertices[0].x, TopVertices[0].y));
	}

	BatchLines.push_back(Line(SourceColor, BottomVertices[0].x, BottomVertices[0].y, BottomVertices[1].x, BottomVertices[1].y));
	BatchLines.push_back(Line(SourceColor, BottomVertices[1].x, BottomVertices[1].y, BottomVertices[2].x, BottomVertices[2].y));
	BatchLines.push_back(Line(SourceColor, BottomVertices[2].x, BottomVertices[2].y, BottomVertices[3].x, BottomVertices[3].y));
	BatchLines.push_back(Line(SourceColor, BottomVertices[3].x, BottomVertices[3].y, BottomVertices[0].x, BottomVertices[0].y));

	if (EnemyHealth != 100)
	{
		BatchLines.push_back(Line(SourceColorBG, TopVertices[0].x, TopVertices[0].y, IntermediateVertices[0].x, IntermediateVertices[0].y));
		BatchLines.push_back(Line(SourceColorBG, TopVertices[1].x, TopVertices[1].y, IntermediateVertices[1].x, IntermediateVertices[1].y));
		BatchLines.push_back(Line(SourceColorBG, TopVertices[2].x, TopVertices[2].y, IntermediateVertices[2].x, IntermediateVertices[2].y));
		BatchLines.push_back(Line(SourceColorBG, TopVertices[3].x, TopVertices[3].y, IntermediateVertices[3].x, IntermediateVertices[3].y));

		BatchLines.push_back(Line(SourceColor, IntermediateVertices[0].x, IntermediateVertices[0].y, BottomVertices[0].x, BottomVertices[0].y));
		BatchLines.push_back(Line(SourceColor, IntermediateVertices[1].x, IntermediateVertices[1].y, BottomVertices[1].x, BottomVertices[1].y));
		BatchLines.push_back(Line(SourceColor, IntermediateVertices[2].x, IntermediateVertices[2].y, BottomVertices[2].x, BottomVertices[2].y));
		BatchLines.push_back(Line(SourceColor, IntermediateVertices[3].x, IntermediateVertices[3].y, BottomVertices[3].x, BottomVertices[3].y));
	}
	else
	{
		BatchLines.push_back(Line(SourceColor, TopVertices[0].x, TopVertices[0].y, BottomVertices[0].x, BottomVertices[0].y));
		BatchLines.push_back(Line(SourceColor, TopVertices[1].x, TopVertices[1].y, BottomVertices[1].x, BottomVertices[1].y));
		BatchLines.push_back(Line(SourceColor, TopVertices[2].x, TopVertices[2].y, BottomVertices[2].x, BottomVertices[2].y));
		BatchLines.push_back(Line(SourceColor, TopVertices[3].x, TopVertices[3].y, BottomVertices[3].x, BottomVertices[3].y));
	}

	BatchDrawLines(&BatchLines);
}

void EntityESP::RenderSkeleton()
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();

	//Checks if its an enemy
	auto bIsEnemy = pLocal->GetTeamNum() != m_pEntity->GetTeamNum();

	int EnemyHealth = m_pEntity->GetHealth();
	float HealthFrac = EnemyHealth / 100.0f;

	Vector4D EnemyColor;
	Vector4D AllyColor(Config.g_iChams_TeamColor[0], Config.g_iChams_TeamColor[1], Config.g_iChams_TeamColor[2], 255.0f);

	if (HealthFrac > 0.5f)
		EnemyColor = Vector4D(255.0f * (1 - HealthFrac) * 2.0f, 255.0f, 0, 255.0f);
	else
		EnemyColor = Vector4D(255.0f, 255.0f * HealthFrac * 2.0f, 0, 255.0f);

	//Get the appropriate drawing color
	auto SourceColor = bIsEnemy ?
		Color(int(EnemyColor.x), int(EnemyColor.y), int(EnemyColor.z), int(EnemyColor.w)) :
		Color(int(AllyColor.x), int(AllyColor.y), int(AllyColor.z), int(AllyColor.w));

	std::vector<Line> BatchLines;

	auto Mdl = m_pEntity->GetModel();
	if (!Mdl) return;
	studiohdr_t* pStudioHdr = Interfaces::ModelInfo()->GetStudioModel(Mdl);
	if (!pStudioHdr) return;

	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->pBone(j);
		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			Vector vChild = m_pEntity->GetBone(j);
			Vector vParent = m_pEntity->GetBone(pBone->parent);
			Vector sParent, sChild;

			if (Utils::WorldToScreen(vParent, sParent) && Utils::WorldToScreen(vChild, sChild))
				BatchLines.push_back(Line(SourceColor, sParent.x, sParent.y, sChild.x, sChild.y));
		}
	}

	BatchDrawLines(&BatchLines);

	Vector vHeadPos;
	Vector vHeadTop;
	Utils::WorldToScreen(m_pEntity->GetHeadCenterPos(), vHeadPos);
	Utils::WorldToScreen(m_pEntity->GetHeadCenterPos() + SourceEngine::Vector(0, 0, 4), vHeadTop);
	DrawOutlinedCircle(Vector2D(vHeadPos.x, vHeadPos.y), SourceColor, vHeadTop.DistTo(vHeadPos));
}

void EntityESP::RenderName()
{
    using namespace SourceEngine;

    auto pLocal = C_CSPlayer::GetLocalPlayer();

    if(!pLocal) return;

    //Checks if its an enemy
    auto bIsEnemy = pLocal->GetTeamNum() != m_pEntity->GetTeamNum();

	int EnemyHealth = m_pEntity->GetHealth();
	float HealthFrac = EnemyHealth / 100.0f;

	Vector4D EnemyColor;
	Vector4D AllyColor(Config.g_iChams_TeamColor[0], Config.g_iChams_TeamColor[1], Config.g_iChams_TeamColor[2], 255.0f);

	if (HealthFrac > 0.5f)
		EnemyColor = Vector4D(255.0f * (1 - HealthFrac) * 2.0f, 255.0f, 0, 255.0f);
	else
		EnemyColor = Vector4D(255.0f, 255.0f * HealthFrac * 2.0f, 0, 255.0f);

	auto SourceColor = bIsEnemy ?
		Color(int(EnemyColor.x), int(EnemyColor.y), int(EnemyColor.z), int(EnemyColor.w)) :
		Color(int(AllyColor.x), int(AllyColor.y), int(AllyColor.z), int(AllyColor.w));

	auto vOrigin = m_pEntity->GetOrigin();
	auto vHead = m_pEntity->GetHeadCenterPos();
	auto pHead = pLocal->GetHeadCenterPos();
	auto dist = pHead.DistTo(vHead);
	auto distoffset = -(dist / 100.0f);
	if (distoffset < -7.0f) distoffset = -7.0f;
	float height = std::fabs((vHead.z + 10) - vOrigin.z);

    Vector vScreenOrigin;

    if(Utils::WorldToScreen(vOrigin + Vector(0, 0, height), vScreenOrigin))
	{
        player_info_t pInfo;
        Interfaces::Engine()->GetPlayerInfo(m_pEntity->EntIndex(), &pInfo);
		DrawStringOutlined(Font[FONT_ESP], Vector2D(vScreenOrigin.x, vScreenOrigin.y + distoffset - 11), SourceColor, 3, pInfo.szName);
		DrawStringOutlined(Font[FONT_ESP], Vector2D(vScreenOrigin.x, vScreenOrigin.y + distoffset), SourceColor, 3, std::to_string(EnemyHealth).c_str());
    }
}

void EntityESP::RenderWeaponName()
{
	auto ActiveWeapon = m_pEntity->GetActiveWeapon();
	if (!ActiveWeapon) return;

	std::string WeaponName = ActiveWeapon->GetWeaponName();
	if (!ActiveWeapon->IsKnife() && !ActiveWeapon->IsGrenade() && !ActiveWeapon->IsC4())
	{
		WeaponName += XorStr(" [");
		WeaponName += std::to_string(ActiveWeapon->GetClip1());
		WeaponName += XorStr("]");
	}

	using namespace SourceEngine;

	auto pLocal = C_CSPlayer::GetLocalPlayer();

	if (!pLocal) return;

	//Checks if its an enemy
	auto bIsEnemy = pLocal->GetTeamNum() != m_pEntity->GetTeamNum();

	int EnemyHealth = m_pEntity->GetHealth();
	float HealthFrac = EnemyHealth / 100.0f;

	Vector4D EnemyColor;
	Vector4D AllyColor(Config.g_iChams_TeamColor[0], Config.g_iChams_TeamColor[1], Config.g_iChams_TeamColor[2], 255.0f);

	if (HealthFrac > 0.5f)
		EnemyColor = Vector4D(255.0f * (1 - HealthFrac) * 2.0f, 255.0f, 0, 255.0f);
	else
		EnemyColor = Vector4D(255.0f, 255.0f * HealthFrac * 2.0f, 0, 255.0f);

	auto SourceColor = bIsEnemy ?
		Color(int(EnemyColor.x), int(EnemyColor.y), int(EnemyColor.z), int(EnemyColor.w)) :
		Color(int(AllyColor.x), int(AllyColor.y), int(AllyColor.z), int(AllyColor.w));

	auto vOrigin = m_pEntity->GetOrigin();

	Vector vScreenOrigin;
	if (Utils::WorldToScreen(vOrigin, vScreenOrigin))
	{
		player_info_t pInfo;
		Interfaces::Engine()->GetPlayerInfo(m_pEntity->EntIndex(), &pInfo);
		DrawStringOutlined(Font[FONT_ESP], Vector2D(vScreenOrigin.x, vScreenOrigin.y), SourceColor, 3, WeaponName.c_str());
	}
}
