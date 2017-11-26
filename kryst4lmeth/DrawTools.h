/////////////////////////////////////////
// zMenu 1.0, Customized for Mutiny.pw //
///////// Made by toast/zzzzz ///////////
/////////////////////////////////////////

#pragma once
#include "SourceEngine\SDK.hpp"
#include "Options.hpp"
#include "XorStr.hpp"
#include <vector>

void InitializeFonts();

extern unsigned long Font[14];

enum Fonts {
	FONT_ESP,
	FONT_BAR,
	FONT_FRAMETITLE,
	FONT_CHECKBOX,
	FONT_AREA,
	FONT_SEPERATOR,
	FONT_TABSELECTION,
	FONT_DROPDOWN,
	FONT_LABEL,
	FONT_SMALLLABEL,
	FONT_BUTTON,
	FONT_SLIDER,
	FONT_CLOCK,
	FONT_TOOLTIP
};

enum Font_Position
{
	Centered_X = 1,
	Centered_Y = 2,
};

struct Line
{
	Line(Color c, int nx1, int ny1, int nx2, int ny2) : cColor(c), x1(nx1), y1(ny1), x2(nx2), y2(ny2) {}
	int x1, x2, y1, y2;
	SourceEngine::Color cColor;
};

namespace DrawTools
{
	void DrawCircle(Vector2D Center, Color c, float rad, int segments);
	void DrawOutlinedCircle(Vector2D Center, Color c, int rad);
	void DrawFilledQuad(Vector2D p0, Vector2D p1, Vector2D p2, Vector2D p3, Color c);
	void DrawFilledQuadOneSided(Vector2D p0, Vector2D p1, Vector2D p2, Vector2D p3, Color c);
	void DrawTextureByteArray(Vector2D p0, Vector2D p1, Vector2D p2, Vector2D p3, const unsigned char* pRawRGBAData);
	void DrawIntRect(IntRect r, Color c);
	void DrawGradientRect(IntRect r, Color c1, Color c2, Color c3, Color c4);
	void DrawGradientHorizRect(IntRect r, Color c1, Color c2, int div = 1);
	void DrawGradientVertRect(IntRect r, Color c1, Color c2, int div = 1);
	void CreateFont(Fonts fIndex, const char* pFontName, int iSize, int iWeight, int iFlags);
	void GetTextSize(unsigned long& Font, int& w, int& h, const char* strText, ...);
	void DrawString(unsigned long& Font, Vector2D pos, Color c, unsigned int flags, const char* strText, ...);
	void DrawStringOutlined(unsigned long& Font, Vector2D pos, Color c, unsigned int flags, const char* strText, ...);

	// If you want to apply an outline to a bunch of drawn lines without overlapping
	void BatchDrawLines(std::vector<Line>* Lines);
};

