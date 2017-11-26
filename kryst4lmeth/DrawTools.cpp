/////////////////////////////////////////
// zMenu 1.0, Customized for Mutiny.pw //
///////// Made by toast/zzzzz ///////////
/////////////////////////////////////////

#include "DrawTools.h"
#include <memory>

void InitializeFonts()
{
	DrawTools::CreateFont(FONT_ESP, XorStr("DroidSans"), 12, 50, FONTFLAG_ANTIALIAS);
	DrawTools::CreateFont(FONT_BAR, XorStr("DroidSans"), 12, 50, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	DrawTools::CreateFont(FONT_LABEL, XorStr("Tahoma"), 32, 50, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	DrawTools::CreateFont(FONT_SMALLLABEL, XorStr("Tahoma"), 14, 50, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
}

unsigned long Font[14];

class TextureHolder
{
public:
	typedef std::shared_ptr<TextureHolder> Ptr;
	TextureHolder()
		: m_pSurface(Interfaces::MatSurface()), m_iH(0), m_iW(0), m_bgColor(255, 255, 255, 255), m_bValid(false)
	{ };

	TextureHolder(const unsigned char* pRawRGBAData, unsigned int W, unsigned int H)
		: m_pSurface(Interfaces::MatSurface()), m_iH(H), m_iW(W), m_bgColor(255, 255, 255, 255), m_bValid(false)
	{
		m_iTexture = m_pSurface->CreateNewTextureID(true);
		if (!m_iTexture)
			return;
		m_pSurface->DrawSetTextureRGBA(m_iTexture, pRawRGBAData, W, H);
		m_bValid = true;
	};

	bool IsValid() const
	{
		return m_bValid;
	};

	int GetTextureId() const
	{
		return m_iTexture;
	};

	bool Draw(int x, int y, float scale = 1.0)
	{
		if (!m_pSurface->IsTextureIDValid(m_iTexture))
			return false;
		m_pSurface->DrawSetColor(m_bgColor);
		m_pSurface->DrawSetTexture(m_iTexture);
		m_pSurface->DrawTexturedRect(x, y, x + m_iW * scale, y + m_iH * scale);
		return true;
	};

protected:
	unsigned int m_iTexture;
	unsigned int m_iW, m_iH;
	Color  m_bgColor;
	bool   m_bValid;

	SourceEngine::ISurface* m_pSurface;
};

namespace DrawTools
{
	void DrawCircle(Vector2D Center, Color c, float rad, int segments)
	{
		float ang_step = (2.0f * M_PI) / (float)segments;
		Interfaces::MatSurface()->DrawSetColor(c);
		for (float Angle = 0; Angle < 2.0f * M_PI; Angle += ang_step)
		{
			Vector2D pos1 = Vector2D(cosf(Angle), sinf(Angle)) * rad + Center;
			Vector2D pos2 = Vector2D(cosf(Angle + ang_step), sinf(Angle + ang_step)) * rad + Center;
			Interfaces::MatSurface()->DrawLine(pos1.x, pos1.y, pos2.x, pos2.y);
		}
	}

	void DrawOutlinedCircle(Vector2D Center, Color c, int rad)
	{
		int res = (Config.g_iESPQuality + 1) * 12;
		if (Config.g_iESPQuality == 2)
		{
			DrawCircle(Center + Vector2D(-1, 0), Color(0, 0, 0, 255), rad, res);
			DrawCircle(Center + Vector2D(1, 0), Color(0, 0, 0, 255), rad, res);
			DrawCircle(Center + Vector2D(0, -1), Color(0, 0, 0, 255), rad, res);
			DrawCircle(Center + Vector2D(0, 1), Color(0, 0, 0, 255), rad, res);
		}
		DrawCircle(Center, c, rad, res);
	}

	void DrawFilledQuad(Vector2D p0, Vector2D p1, Vector2D p2, Vector2D p3, Color c)
	{
		Vertex_t verts[4];
		static int Texture = Interfaces::MatSurface()->CreateNewTextureID(true);
		byte buffer[4] = { 255, 255, 255, 255 };
		Interfaces::MatSurface()->DrawSetTextureRGBA(Texture, buffer, 1, 1);
		Interfaces::MatSurface()->DrawSetColor((byte)c.r(), (byte)c.g(), (byte)c.b(), (byte)c.a());
		Interfaces::MatSurface()->DrawSetTexture(Texture); 
		
		verts[0].Init(p0);
		verts[1].Init(p1);
		verts[2].Init(p2);
		verts[3].Init(p3);
		Interfaces::MatSurface()->DrawTexturedPolygon(4, verts);

		verts[3].Init(p0);
		verts[2].Init(p1);
		verts[1].Init(p2);
		verts[0].Init(p3);
		Interfaces::MatSurface()->DrawTexturedPolygon(4, verts);
	}

	void DrawFilledQuadOneSided(Vector2D p0, Vector2D p1, Vector2D p2, Vector2D p3, Color c)
	{
		Vertex_t verts[4];
		static int Texture = Interfaces::MatSurface()->CreateNewTextureID(true);
		byte buffer[4] = { 255, 255, 255, 255 };
		Interfaces::MatSurface()->DrawSetTextureRGBA(Texture, buffer, 1, 1);
		Interfaces::MatSurface()->DrawSetColor((byte)c.r(), (byte)c.g(), (byte)c.b(), (byte)c.a());
		Interfaces::MatSurface()->DrawSetTexture(Texture);

		verts[0].Init(p0);
		verts[1].Init(p1);
		verts[2].Init(p2);
		verts[3].Init(p3);
		Interfaces::MatSurface()->DrawTexturedPolygon(4, verts);
	}

	void DrawTextureByteArray(Vector2D p0, Vector2D p1, Vector2D p2, Vector2D p3, const unsigned char* pRawRGBAData)
	{
		Vertex_t verts[4];
		static int Texture = Interfaces::MatSurface()->CreateNewTextureID(true);
		Interfaces::MatSurface()->DrawSetTextureRGBA(Texture, pRawRGBAData, 1, 1);
		Interfaces::MatSurface()->DrawSetColor(255, 255, 255, 255);
		Interfaces::MatSurface()->DrawSetTexture(Texture);

		verts[0].Init(p0);
		verts[1].Init(p1);
		verts[2].Init(p2);
		verts[3].Init(p3);
		Interfaces::MatSurface()->DrawTexturedPolygon(4, verts);
	}

	void DrawGradientRect(IntRect r, Color c1, Color c2, Color c3, Color c4)
	{
		for (int h = 0; h < r.h; h++)
		{
			for (int w = 0; w < r.w; w++)
			{
				float wblend = w / (float)r.w;
				float hblend = h / (float)r.h;

				Color xb1 = c2 * wblend + c1 * (1.0f - wblend);
				Color xb2 = c3 * wblend + c4 * (1.0f - wblend);
				Color Blend = xb2 * hblend + xb1 * (1.0f - hblend);
				DrawIntRect(IntRect{ r.x0 + w, r.y0 + h, 1, 1 }, Blend);
			}
		}
	}

	void DrawGradientHorizRect(IntRect r, Color c1, Color c2, int div)
	{
		for (int w = 0; w < r.w; w += div)
		{
			float wblend = w / (float)r.w;
			Color Blend = c2 * wblend + c1 * (1.0f - wblend);
			DrawIntRect(IntRect{ r.x0, r.y0, div, r.h }, Blend);
		}
	}

	void DrawGradientVertRect(IntRect r, Color c1, Color c2, int div)
	{
		for (int h = 0; h < r.h; h += div)
		{
			float hblend = h / (float)r.h;
			Color Blend = c2 * hblend + c1 * (1.0f - hblend);
			DrawIntRect(IntRect{ r.x0, r.y0 + h, r.w, div }, Blend);
		}
	}

	void DrawIntRect(IntRect r, Color c)
	{
		Interfaces::MatSurface()->DrawSetColor(c);
		Interfaces::MatSurface()->DrawFilledRect(r.x0, r.y0, r.x0 + r.w, r.y0 + r.h);
	}

	void CreateFont(Fonts fIndex, const char* pFontName, int iSize, int iWeight, int iFlags) {
		Font[fIndex] = Interfaces::MatSurface()->CreateFont();
		Interfaces::MatSurface()->SetFontGlyphSet(Font[fIndex], pFontName, iSize, iWeight, 0, 0, iFlags, 0, 0);
	}

	void GetTextSize(unsigned long& Font, int& w, int& h, const char* strText, ...) {
		char buf[1024];
		wchar_t wbuf[1024];

		va_list vlist;
		va_start(vlist, strText);
		vsprintf(buf, strText, vlist);
		va_end(vlist);

		MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);
		Interfaces::MatSurface()->GetTextSize(Font, wbuf, w, h);
	}

	void DrawString(unsigned long& Font, Vector2D pos, Color c, unsigned int flags, const char* strText, ...)
	{
		char buf[1024];
		wchar_t wbuf[1024];

		va_list vlist;
		va_start(vlist, strText);
		vsprintf(buf, strText, vlist);
		va_end(vlist);

		MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

		int w, h;
		GetTextSize(Font, w, h, strText, vlist);

		if (flags & Centered_X)
			pos.x -= w / 2.0f;
		if (flags & Centered_Y)
			pos.y -= h / 2.0f;

		Interfaces::MatSurface()->DrawSetTextColor(c.r(), c.g(), c.b(), c.a());
		Interfaces::MatSurface()->DrawSetTextPos(pos.x, pos.y);
		Interfaces::MatSurface()->DrawSetTextFont(Font);
		Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
	}

	void DrawStringOutlined(unsigned long& Font, Vector2D pos, Color c, unsigned int flags, const char* strText, ...)
	{
		char buf[1024];
		wchar_t wbuf[1024];

		va_list vlist;
		va_start(vlist, strText);
		vsprintf(buf, strText, vlist);
		va_end(vlist);

		MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

		int w, h;
		GetTextSize(Font, w, h, strText, vlist);

		if (flags & Centered_X)
			pos.x -= w / 2.0f;
		if (flags & Centered_Y)
			pos.y -= h / 2.0f;

		Interfaces::MatSurface()->DrawSetTextFont(Font);
		Interfaces::MatSurface()->DrawSetTextColor(0, 0, 0, 255);
		
		if (Config.g_iESPQuality > 0)
		{
			Interfaces::MatSurface()->DrawSetTextPos(pos.x + 1, pos.y);
			Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
			Interfaces::MatSurface()->DrawSetTextPos(pos.x - 1, pos.y);
			Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
			Interfaces::MatSurface()->DrawSetTextPos(pos.x, pos.y + 1);
			Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
			Interfaces::MatSurface()->DrawSetTextPos(pos.x, pos.y - 1);
			Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
		}
		if (Config.g_iESPQuality > 1)
		{
			Interfaces::MatSurface()->DrawSetTextPos(pos.x + 1, pos.y + 1);
			Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
			Interfaces::MatSurface()->DrawSetTextPos(pos.x - 1, pos.y + 1);
			Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
			Interfaces::MatSurface()->DrawSetTextPos(pos.x + 1, pos.y - 1);
			Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
			Interfaces::MatSurface()->DrawSetTextPos(pos.x - 1, pos.y - 1);
			Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
		}
		Interfaces::MatSurface()->DrawSetTextColor(c.r(), c.g(), c.b(), c.a());
		Interfaces::MatSurface()->DrawSetTextPos(pos.x, pos.y);
		Interfaces::MatSurface()->DrawPrintText(wbuf, wcslen(wbuf), (FontDrawType)0);
	}

	void BatchDrawLines(std::vector<Line>* Lines)
	{
		if (Config.g_iESPQuality == 2)
		{
			Interfaces::MatSurface()->DrawSetColor(0, 0, 0, 255);
			for (auto i = Lines->begin(); i < Lines->end(); i++)
			{
				Interfaces::MatSurface()->DrawLine((*i).x1 + 1, (*i).y1, (*i).x2 + 1, (*i).y2);
				Interfaces::MatSurface()->DrawLine((*i).x1 - 1, (*i).y1, (*i).x2 - 1, (*i).y2);
				Interfaces::MatSurface()->DrawLine((*i).x1, (*i).y1 + 1, (*i).x2, (*i).y2 + 1);
				Interfaces::MatSurface()->DrawLine((*i).x1, (*i).y1 - 1, (*i).x2, (*i).y2 - 1);
			}
		}
		for (auto i = Lines->begin(); i < Lines->end(); i++)
		{
			Interfaces::MatSurface()->DrawSetColor((*i).cColor);
			Interfaces::MatSurface()->DrawLine((*i).x1, (*i).y1, (*i).x2, (*i).y2);
		}
	}
}
