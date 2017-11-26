/////////////////////////////////////////
// zMenu 1.2, kryst4lmeth: Open Source //
//////// Made by toast/itoasti //////////
/////////////////////////////////////////

/*
	A higly modular and flexible ISurface menu.
	This menu uses a parenting system where
	children will inherit traits from their
	parent automatically.  This minimizes effort
	and maximizes organization. Traits such as position,
	visibility and opacity will be applied to an
	object and all it's children.  In the case of opacity,
	if a parent had an opacity of 128 and the child had
	an opacity of 128, the result would be a child with
	an visual opacity of 64.  Think of it like
	multiplying fractions.
*/

#include "Menu.h"
#include "AutoShoot.h"
#include "NameChanger.h"
#include "AntiAim.h"
#include <ctime>

using namespace DrawTools;

std::clock_t start;

namespace Menu
{
	//struct IntRect { int x0, y0, w, h; };

	enum MenuTypes
	{
		Menu_BaseRenderable = 0,
		Menu_Window,
		Menu_Tab,
		Menu_SubTab,
		Menu_VerticalLayout,
		Menu_Interactable,
		Menu_Checkbox,
		Menu_Button,
		Menu_Combobox,
		Menu_Slider,
	};

	bool ShouldDraw;
	bool IsMouseDown;
	bool IsMouseClicked;
	Vector2D ScreenSize;
	Vector2D CursorPos;
	Vector2D CursorDeltaPos;
	std::string ToolTipString;
	bool InvertedTabs = true;
	int MenuAlpha = 0;

	std::vector<CBaseRenderable*> BaseObjects;

	class CBaseRenderable
	{
	public:
		CBaseRenderable() {}
		~CBaseRenderable()
		{
			for (auto i = Children.begin(); i != Children.end(); i++)
				delete (*i);
		}
		virtual MenuTypes GetType() { return Menu_BaseRenderable; }
		virtual int GetVariant() { return 0; }
		virtual void Update()
		{
			for (auto i = Children.begin(); i != Children.end(); i++)
			{
				if ((*i)->GetVisible())
					(*i)->Update();
			}
		}
		virtual void Draw()
		{
			for (auto i = Children.begin(); i != Children.end(); i++)
			{
				if ((*i)->GetVisible())
					(*i)->Draw();
			}
		}
		virtual CBaseRenderable* GetParent() { return Parent; }
		virtual void SetParent(CBaseRenderable* NewParent)
		{
			Parent = NewParent;
		}
		virtual std::vector<CBaseRenderable*>* GetChildren() { return &Children; }
		virtual Vector2D GetPosition(bool Relative = false)
		{
			if (!Parent) return Vector2D(RelativePosition.x, RelativePosition.y);
			else
			{
				if (Relative)
					return Vector2D((int)RelativePosition.x, (int)RelativePosition.y);
				else
					return Parent->GetPosition() + Vector2D((int)RelativePosition.x, (int)RelativePosition.y);
			}
		}
		virtual float GetOpacity()
		{
			if (!Parent) return Opacity;
			else
				return (Parent->GetOpacity() / 255.0f) * Opacity;
		}
		virtual int GetIndex()
		{
			if (!Parent) return 0;
			int c = 0;
			for (auto i = GetParent()->Children.begin(); i != GetParent()->Children.end(); i++)
			{
				if ((*i) == this)
					return c;
				if ((*i)->GetVisible())
					c++;
			}
			return 0;
		}
		virtual void Initialize()
		{
			for (auto i = Children.begin(); i != Children.end(); i++)
			{
				(*i)->Initialize();
			}
		}
		virtual void SetOpacity(int o)
		{
			Opacity = o;
		}
		virtual void SetPosition(Vector2D NewPosition, bool Relative = true)
		{
			if (Relative || !Parent)
				RelativePosition = NewPosition;
			else
				RelativePosition = -Parent->GetPosition() + NewPosition;
		}
		virtual void OnDragged() {};
		virtual void OnClicked() {};
		virtual bool IsCursorInBounds(IntRect Rekt)
		{
			if (CursorPos.x >= Rekt.x0 &&
				CursorPos.x < Rekt.x0 + Rekt.w &&
				CursorPos.y >= Rekt.y0 &&
				CursorPos.y < Rekt.y0 + Rekt.h)
				return true;
			return false;
		}
		virtual void AddChild(CBaseRenderable* Child)
		{
			Children.push_back(Child);
			Child->SetParent(this);
		}
		virtual int GetNumChildrenOfType(MenuTypes Type)
		{
			int c = 0;
			for (auto i = Children.begin(); i != Children.end(); i++)
			{
				if ((*i)->GetType() == Type)
					c++;
			}
			return c;
		}
		virtual int GetNumVisibleChildrenOfType(MenuTypes Type)
		{
			int c = 0;
			for (auto i = Children.begin(); i != Children.end(); i++)
			{
				if ((*i)->GetType() == Type && (*i)->GetVisible())
					c++;
			}
			return c;
		}
		virtual Vector2D GetLabelSize()
		{
			int w, h;
			GetTextSize(Font[FONT_BAR], w, h, Label.c_str());
			return Vector2D(w, h);
		}
		virtual IntRect GetBounds()
		{
			return IntRect();
		}
		virtual void SetVisible(bool V)
		{
			IsVisible = V;
		}
		virtual bool GetVisible()
		{
			return IsVisible;
		}
	protected:
		std::vector<CBaseRenderable*> Children;
		CBaseRenderable* Parent = nullptr;
		Vector2D RelativePosition = Vector2D(0, 0);
		std::string Label;
		int Opacity = 255;
		bool IsVisible = true;
	};

	class CWindow : public CBaseRenderable
	{
	public:
		CWindow(Vector2D Position, Vector2D Dimensions, int NewHeaderHeight = 14, std::string HeaderString = "") : HeaderHeight(NewHeaderHeight)
		{
			Label = HeaderString;
			// Subtract 8 pixels for the header
			Box = { (int)Position.x, (int)Position.y, (int)Dimensions.x, (int)Dimensions.y - HeaderHeight };
			SetPosition(Position);
		}
		virtual MenuTypes GetType() { return Menu_Window; }
		virtual void Draw()
		{
			// Draw header
			DrawGradientVertRect(IntRect{ Box.x0, Box.y0, Box.w, HeaderHeight }, Color(128, 128, 128, GetOpacity()),  Color(64, 64, 64, GetOpacity()));
			DrawString(Font[FONT_BAR], Vector2D(Box.x0, Box.y0) + Vector2D(2, 1), Color(225, 225, 225, GetOpacity()), 0, Label.c_str());
			// Draw exit button
			DrawGradientVertRect(IntRect{ Box.x0 + Box.w - 12, Box.y0 + 2, 10, 10}, Color(96, 96, 96, GetOpacity()), Color(48, 48, 48, GetOpacity()));
			// Draw window box
			DrawIntRect(IntRect{ Box.x0, Box.y0 + HeaderHeight, Box.w, Box.h }, Color(24, 24, 24, GetOpacity()));
			// Draw children AFTER parent is already drawn
			CBaseRenderable::Draw();
		}
		virtual int GetHeaderHeight()
		{
			return HeaderHeight;
		}
		virtual IntRect GetBounds()
		{
			return IntRect{ Box.x0, Box.y0 + HeaderHeight, Box.w, Box.h };
		}
		virtual void Focus()
		{
			for (auto i = BaseObjects.begin(); i != BaseObjects.end(); i++)
			{
				if ((*i) == this)
				{
					i = BaseObjects.erase(i);
					BaseObjects.push_back((CBaseRenderable*)this);
					break;
				}
			}
		}
		virtual void OnClose()
		{
			SetVisible(false);
		}
		virtual void Update()
		{
			Box.x0 = GetPosition().x;
			Box.y0 = GetPosition().y;
			if (IsCursorInBounds({ Box.x0 + Box.w - 12, Box.y0 + 2, 10, 10 }) && IsMouseClicked)
			{
				OnClose();
				return;
			}
			if (IsCursorInBounds({ Box.x0, Box.y0, Box.w, HeaderHeight }) && IsMouseClicked)
			{
				IsBeingDragged = true;
				Focus();
			}
			if (IsCursorInBounds({ Box.x0 + Box.w - 4, Box.y0 + Box.h + HeaderHeight - 4, 8, 8 }) && IsMouseClicked)
			{
				IsBeingResized = true;
				Focus();
			}
			if (!IsMouseDown && IsBeingDragged)
				IsBeingDragged = false;
			if (!IsMouseDown && IsBeingResized)
				IsBeingResized = false;
			if (IsBeingDragged)
				OnDragged();
			if (IsBeingResized)
				OnResized();

			CBaseRenderable::Update();
		}
		virtual void OnDragged()
		{
			Vector2D NewPosition = GetPosition() + CursorDeltaPos;
			if (NewPosition.x < -Box.w + 40) NewPosition.x = -Box.w + 40;
			if (NewPosition.x > ScreenSize.x - 40) NewPosition.x = ScreenSize.x - 40;
			if (NewPosition.y < 0) NewPosition.y = 0;
			if (NewPosition.y > ScreenSize.y - 40) NewPosition.y = ScreenSize.y - 40;
			SetPosition(NewPosition);
			Interfaces::MatSurface()->SurfaceSetCursorPos(NewPosition.x, NewPosition.y);
			IsMouseClicked = false;
		}
		virtual void OnResized()
		{
			if (CursorPos.x < 2) CursorPos.x = 2;
			if (CursorPos.y < 2) CursorPos.y = 2;
			if (CursorPos.x > ScreenSize.x - 2) CursorPos.x = ScreenSize.x - 2;
			if (CursorPos.y > ScreenSize.y - 2) CursorPos.y = ScreenSize.y - 2;
			Box = { Box.x0, Box.y0, (int)CursorPos.x - Box.x0, (int)CursorPos.y - (Box.y0 + HeaderHeight) };
			if (Box.h < 240) Box.h = 240;
			if (Box.w < 320) Box.w = 320;
			if (Box.h > 600) Box.h = 600;
			if (Box.w > 800) Box.w = 800;
		}
	protected:
		IntRect Box;
		bool IsBeingDragged = false;
		bool IsBeingResized = false;
		int HeaderHeight = 12;
	};

	class CMainWindow : public CWindow
	{
	public:
		CMainWindow(Vector2D Position, Vector2D Dimensions, int NewHeaderHeight = 14, std::string HeaderString = "") : CWindow(Position, Dimensions, NewHeaderHeight, HeaderString) {}
		virtual void OnClose()
		{
			Menu::ShouldDraw = false;
		}
		virtual void OnResized()
		{
			Box = { Box.x0, Box.y0, (int)CursorPos.x - Box.x0, (int)CursorPos.y - (Box.y0 + HeaderHeight)};
			if (Box.h < 280) Box.h = 280;
			if (Box.w < 440) Box.w = 440;
			if (Box.h > 600) Box.h = 600;
			if (Box.w > 800) Box.w = 800;
		}
	};

	class CTab : public CBaseRenderable
	{
	public:
		CTab(std::string Category, int Width = 28) : TabWidth(Width) { Label = Category; }
		virtual void Initialize()
		{
			if (!GetIndex()) IsActive = true;
			CBaseRenderable::Initialize();
		}
		virtual void Update()
		{
			if (!GetParent()) return;
			IntRect Box = GetBounds();
			int TabHeight = ((Box.y0 + Box.h) - (Box.y0)) / (float)GetParent()->GetNumVisibleChildrenOfType(Menu_Tab);

			IntRect ClickableTab;
			if (InvertedTabs)
				ClickableTab = { Box.x0 - TabWidth - 4, Box.y0 + TabHeight * GetIndex(), TabWidth, TabHeight };
			else
				ClickableTab = { Box.x0 - TabWidth, Box.y0 + TabHeight * GetIndex(), TabWidth, TabHeight };

			if (IsCursorInBounds(ClickableTab) && IsMouseClicked)
			{
				IsActive = true;
				for (auto i = GetParent()->GetChildren()->begin(); i != GetParent()->GetChildren()->end(); i++)
				{
					if ((*i)->GetType() == Menu_Tab)
					{
						auto OtherTab = static_cast<CTab*>(*i);
						if (OtherTab != this)
							OtherTab->IsActive = false;
					}
				}
				IsMouseClicked = false;
			}
			else if (IsCursorInBounds(ClickableTab))
				ToolTipString = Label;
			if (IsActive)
				CBaseRenderable::Update();
		}
		virtual MenuTypes GetType() { return Menu_Tab; }
		virtual void Draw()
		{
			IntRect Box = GetBounds();
			auto TabColor = IsActive ? Color(48, 48, 48, GetOpacity()) : Color(32, 32, 32, GetOpacity());
			if (IsActive)
			{
				DrawIntRect(IntRect{ Box.x0, Box.y0, Box.w, Box.h }, TabColor);
				CBaseRenderable::Draw();
			}

			// Draw the actual tab
			int TabHeight = ((Box.y0 + Box.h ) - (Box.y0)) / (float)GetParent()->GetNumVisibleChildrenOfType(Menu_Tab);
			Vector2D Center;

			if (InvertedTabs)
			{
				DrawFilledQuadOneSided(Vector2D(Box.x0 - 4, Box.y0 + TabHeight * (GetIndex() + 1) - TabHeight / 8.0f),
					Vector2D(Box.x0 - TabWidth - 4, Box.y0 + TabHeight * (GetIndex() + 1)),
					Vector2D(Box.x0 - TabWidth - 4, Box.y0 + TabHeight * GetIndex()),
					Vector2D(Box.x0 - 4, Box.y0 + TabHeight * GetIndex() + TabHeight / 8.0f),
					TabColor);
				Center = Vector2D(Box.x0 - TabWidth / 2 - 4, Box.y0 + TabHeight * GetIndex() + TabHeight / 2.0f);
			}
			else
			{
				DrawFilledQuadOneSided(Vector2D(Box.x0 - TabWidth, Box.y0 + TabHeight * GetIndex() + TabHeight / 8.0f),
					Vector2D(Box.x0, Box.y0 + TabHeight * GetIndex()),
					Vector2D(Box.x0, Box.y0 + TabHeight * (GetIndex() + 1)),
					Vector2D(Box.x0 - TabWidth, Box.y0 + TabHeight * (GetIndex() + 1) - TabHeight / 8.0f),
					TabColor);
				Center = Vector2D(Box.x0 - TabWidth / 2, Box.y0 + TabHeight * GetIndex() + TabHeight / 2.0f);
			}
			DrawString(Font[FONT_LABEL], Center, Color(225, 225, 225, GetOpacity()), 3, Label.substr(0, 1).c_str());
		}
		virtual IntRect GetBounds()
		{
			auto WindowParent = static_cast<CWindow*>(GetParent());
			IntRect Box = WindowParent->GetBounds();
			IntRect Bounds = { Box.x0 + TabWidth + 4, Box.y0 + 4, Box.w - 8 - TabWidth, Box.h - 8 };
			return Bounds;
		}
	protected:
		IntRect TabArea;
		int TabWidth;
		bool IsActive = false;
	};

	class CSubTab : public CBaseRenderable
	{
	public:
		CSubTab(std::string Category, int Width = 20) : TabWidth(Width) { Label = Category; }
		virtual void Initialize()
		{
			if (!GetIndex()) IsActive = true;
			CBaseRenderable::Initialize();
		}
		virtual void Update()
		{
			if (!GetParent()) return;
			IntRect Box = GetBounds();
			int TabHeight = ((Box.x0 + Box.w) - (Box.x0)) / (float)GetParent()->GetNumVisibleChildrenOfType(Menu_SubTab);

			IntRect ClickableTab{ Box.x0 + TabHeight * GetIndex(), Box.y0 - TabWidth, TabHeight, TabWidth };
			if (IsCursorInBounds(ClickableTab) && IsMouseClicked)
			{
				IsActive = true;
				for (auto i = GetParent()->GetChildren()->begin(); i != GetParent()->GetChildren()->end(); i++)
				{
					if ((*i)->GetType() == Menu_SubTab)
					{
						auto OtherTab = static_cast<CSubTab*>(*i);
						if (OtherTab != this)
							OtherTab->IsActive = false;
					}
				}
				IsMouseClicked = false;
			}
			else if (IsCursorInBounds(ClickableTab))
				ToolTipString = Label;
			if (IsActive)
				CBaseRenderable::Update();
		}
		virtual MenuTypes GetType() { return Menu_SubTab; }
		virtual void Draw()
		{
			IntRect Box = GetBounds();
			auto TabColor = IsActive ? Color(36, 36, 36, GetOpacity()) : Color(28, 28, 28, GetOpacity());
			if (IsActive)
			{
				DrawIntRect(IntRect{ Box.x0, Box.y0, Box.w, Box.h }, TabColor);
				CBaseRenderable::Draw();
			}

			// Draw the actual tab
			int TabHeight = ((Box.x0 + Box.w) - (Box.x0)) / (float)GetParent()->GetNumVisibleChildrenOfType(Menu_SubTab);
			DrawFilledQuadOneSided(Vector2D(Box.x0 + TabHeight * (GetIndex() + 1), Box.y0),
				Vector2D(Box.x0 + TabHeight * GetIndex(), Box.y0),
				Vector2D(Box.x0 + TabHeight * GetIndex() + TabHeight / 8.0f, Box.y0 - TabWidth),
				Vector2D(Box.x0 + TabHeight * (GetIndex() + 1) - TabHeight / 8.0f, Box.y0 - TabWidth),
				TabColor);
			
			Vector2D Center = Vector2D(Box.x0 + TabHeight * GetIndex() + TabHeight / 2.0f, Box.y0 - TabWidth / 2);
			DrawString(Font[FONT_SMALLLABEL], Center, Color(225, 225, 225, GetOpacity()), 3, Label.c_str());
		}
		virtual IntRect GetBounds()
		{
			IntRect Box = GetParent()->GetBounds();
			IntRect Bounds = { Box.x0 + 4, Box.y0 + TabWidth + 4, Box.w - 8, Box.h - 8 - TabWidth };
			return Bounds;
		}
	protected:
		IntRect TabArea;
		int TabWidth;
		bool IsActive = false;
	};

	class CVerticalLayout : public CBaseRenderable
	{
	public:
		CVerticalLayout(std::string Title) : CBaseRenderable() { Label = Title; }
		virtual MenuTypes GetType() { return Menu_VerticalLayout; }
		virtual void Update()
		{
			if (!GetParent()) return;
			if (GetParent()->GetType() != Menu_Tab && GetParent()->GetType() != Menu_SubTab && GetParent()->GetType() != Menu_Window) return;
			int z = 0;
			for (int c = 0; c < Children.size(); c++)
			{
				if (!Children[c]->GetVisible()) continue;
				// Subtract another 16 pixels because the items are top-left aligned
				int BoxHeight = GetBounds().h - 28;
				int Vol = GetAdaptiveVolume();
				Children[c]->SetPosition(Vector2D(16, 8 + z * ((float)BoxHeight / Vol) + 8));
				z++;
			}
			CBaseRenderable::Update();
		}
		virtual void Draw()
		{
			auto Box = GetBounds();
			DrawIntRect(IntRect{ Box.x0 + 2, Box.y0 + 1, Box.w, Box.h }, Color(32, 32, 32, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0, Box.y0 - 1, Box.w, Box.h }, Color(64, 64, 64, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 3, Box.y0 + 2, Box.w - 6, Box.h - 6 }, Color(42, 42, 42, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 5, Box.y0 + 4, Box.w - 10, Box.h - 10 }, Color(48, 48, 48, GetOpacity()));
			int w, h;
			GetTextSize(Font[FONT_BAR], w, h, Label.c_str());
			DrawIntRect(IntRect{ Box.x0 + 8, Box.y0 - 4, w + 5, h - 1 }, Color(64, 64, 64, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 9, Box.y0 - 3, w + 3, h - 3 }, Color(48, 48, 48, GetOpacity()));
			DrawString(Font[FONT_BAR], Vector2D(Box.x0 + 10, Box.y0 - 4), Color(225, 225, 225, GetOpacity()), 0, Label.c_str());

			// Reverse iteration drawing
			// Stops bottom items drawing over dropdown boxes
			for (int i = Children.size() - 1; i >= 0; i--)
			{
				if (!Children[i]->GetVisible()) continue;
				Children[i]->Draw();
			}
		}
		virtual int GetAdaptiveVolume()
		{
			int i = 0;
			for (int c = 0; c < Children.size(); c++)
			{
				if (!Children[c]->GetVisible()) continue;
				i++;
			}
			return i;
		}
		virtual IntRect GetBounds()
		{
			if (!GetParent()) return IntRect();
			int NumVerticalLayouts = GetParent()->GetNumVisibleChildrenOfType(Menu_VerticalLayout);
			IntRect Box = GetParent()->GetBounds();
			int Width = (int)((Box.w - 16 - 8 * (NumVerticalLayouts - 1)) / (float)NumVerticalLayouts);
			int Height = Box.h - 16;
			IntRect CurrentBounds = { Box.x0 + 8 + 8 * GetIndex() + Width * GetIndex(), Box.y0 + 8, Width, Height};
			return CurrentBounds;
		}
		virtual Vector2D GetPosition(bool Relative = false)
		{
			if (Relative)
			{
				IntRect Bounds_Parent = GetParent()->GetBounds();
				IntRect Bounds = GetBounds();
				return Vector2D((int)(Bounds.x0 - Bounds_Parent.x0), (int)(Bounds.y0 - Bounds_Parent.y0));
			}
			else
			{
				auto Bounds = GetBounds();
				return Vector2D(Bounds.x0, Bounds.y0);
			}
		}
	};

	class CSplitVerticalLayout : public CBaseRenderable
	{
	public:
		CSplitVerticalLayout(std::string Title1, std::string Title2, int NumTop) : Label2(Title2), NumberOnTop(NumTop) { Label = Title1; }
		virtual MenuTypes GetType() { return Menu_VerticalLayout; }
		virtual int GetVariant() { return 1; }
		virtual void Update()
		{
			if (!GetParent()) return;
			if (GetParent()->GetType() != Menu_Tab && GetParent()->GetType() != Menu_SubTab && GetParent()->GetType() != Menu_Window) return;
			int z = 0;
			for (int c = 0; c < Children.size(); c++)
			{
				if (c < NumberOnTop)
				{
					if (!Children[c]->GetVisible()) continue;
					// Subtract another 16 pixels because the items are top-left aligned
					int BoxHeight = GetBounds(0).h - 28;
					int Vol = GetAdaptiveVolume(0);
					Children[c]->SetPosition(Vector2D(16, 8 + z * ((float)BoxHeight / Vol) + 8));
					z++;
				}
				else
				{
					if (!Children[c]->GetVisible()) continue;
					// Subtract another 16 pixels because the items are top-left aligned
					int BoxHeight = GetBounds(1).h - 28;
					int Vol =  GetAdaptiveVolume(1);
					Children[c]->SetPosition(Vector2D(16, 8 + (GetBounds(1).h + 8) + z * ((float)BoxHeight / Vol) + 8));
					z++;
				}
				if (c == NumberOnTop - 1)
					z = 0;
			}
			CBaseRenderable::Update();
		}
		virtual void Draw()
		{
			auto Box = GetBounds(0);
			DrawIntRect(IntRect{ Box.x0 + 2, Box.y0 + 1, Box.w, Box.h }, Color(32, 32, 32, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0, Box.y0 - 1, Box.w, Box.h }, Color(64, 64, 64, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 3, Box.y0 + 2, Box.w - 6, Box.h - 6 }, Color(42, 42, 42, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 5, Box.y0 + 4, Box.w - 10, Box.h - 10 }, Color(48, 48, 48, GetOpacity()));
			int w, h;
			GetTextSize(Font[FONT_BAR], w, h, Label.c_str());
			DrawIntRect(IntRect{ Box.x0 + 8, Box.y0 - 4, w + 5, h - 1 }, Color(64, 64, 64, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 9, Box.y0 - 3, w + 3, h - 3 }, Color(48, 48, 48, GetOpacity()));
			DrawString(Font[FONT_BAR], Vector2D(Box.x0 + 10, Box.y0 - 4), Color(225, 225, 225, GetOpacity()), 0, Label.c_str());

			Box = GetBounds(1);
			DrawIntRect(IntRect{ Box.x0 + 2, Box.y0 + 1, Box.w, Box.h }, Color(32, 32, 32, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0, Box.y0 - 1, Box.w, Box.h }, Color(64, 64, 64, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 3, Box.y0 + 2, Box.w - 6, Box.h - 6 }, Color(42, 42, 42, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 5, Box.y0 + 4, Box.w - 10, Box.h - 10 }, Color(48, 48, 48, GetOpacity()));
			GetTextSize(Font[FONT_BAR], w, h, Label2.c_str());
			DrawIntRect(IntRect{ Box.x0 + 8, Box.y0 - 4, w + 5, h - 1 }, Color(64, 64, 64, GetOpacity()));
			DrawIntRect(IntRect{ Box.x0 + 9, Box.y0 - 3, w + 3, h - 3 }, Color(48, 48, 48, GetOpacity()));
			DrawString(Font[FONT_BAR], Vector2D(Box.x0 + 10, Box.y0 - 4), Color(225, 225, 225, GetOpacity()), 0, Label2.c_str());

			// Reverse iteration drawing
			// Stops bottom items drawing over dropdown boxes
			for (int i = Children.size() - 1; i >= 0; i--)
				Children[i]->Draw();
		}
		virtual IntRect GetBounds(int SubSection)
		{
			if (!GetParent()) return IntRect();
			int NumVerticalLayouts = GetParent()->GetNumVisibleChildrenOfType(Menu_VerticalLayout);
			IntRect Box = GetParent()->GetBounds();
			int Width = (int)((Box.w - 16 - 8 * (NumVerticalLayouts - 1)) / (float)NumVerticalLayouts);
			int Height = (int)((Box.h - 24) / 2.0f);
			IntRect CurrentBounds = { Box.x0 + 8 + 8 * GetIndex() + Width * GetIndex(), Box.y0 + 8 + 8 * SubSection + Height * SubSection, Width, Height };
			return CurrentBounds;
		}
		virtual int GetAdaptiveVolume(int Section)
		{
			int i = 0;
			if (Section == 0)
			{
				for (int c = 0; c < min(NumberOnTop, Children.size()); c++)
				{
					if (!Children[c]->GetVisible()) continue;
					i++;
				}
			}
			else
			{
				if (Children.size() <= NumberOnTop) return 0;
				for (int c = NumberOnTop; c < Children.size(); c++)
				{
					if (!Children[c]->GetVisible()) continue;
					i++;
				}
			}
			return i;
		}
		virtual IntRect GetBounds()
		{
			if (!GetParent()) return IntRect();
			int NumVerticalLayouts = GetParent()->GetNumVisibleChildrenOfType(Menu_VerticalLayout);
			IntRect Box = GetParent()->GetBounds();
			int Width = (Box.w - 16 - 8 * (NumVerticalLayouts - 1)) / NumVerticalLayouts;
			int Height = Box.h - 16;
			IntRect CurrentBounds = { Box.x0 + 8 + 8 * GetIndex() + Width * GetIndex(), Box.y0 + 8, Width, Height };
			return CurrentBounds;
		}
		virtual Vector2D GetPosition(bool Relative = false, int SubSection = 0)
		{
			if (Relative)
			{
				IntRect Bounds_Parent = GetParent()->GetBounds();
				IntRect Bounds = GetBounds(SubSection);
				return Vector2D((int)(Bounds.x0 - Bounds_Parent.x0), (int)(Bounds.y0 - Bounds_Parent.y0));
			}
			else
			{
				auto Bounds = GetBounds(SubSection);
				return Vector2D(Bounds.x0, Bounds.y0);
			}
		}
		virtual Vector2D GetPosition(bool Relative = false)
		{
			if (Relative)
			{
				auto TabParent = static_cast<CTab*>(GetParent());
				IntRect Bounds_Parent = TabParent->GetBounds();
				IntRect Bounds = GetBounds(0);
				return Vector2D((int)(Bounds.x0 - Bounds_Parent.x0), (int)(Bounds.y0 - Bounds_Parent.y0));
			}
			else
			{
				auto Bounds = GetBounds(0);
				return Vector2D(Bounds.x0, Bounds.y0);
			}
		}
	protected:
		std::string Label2;
		int NumberOnTop;
	};

	class CInteractable : public CBaseRenderable
	{
	public:
		CInteractable() {}
		CInteractable(std::string NewToolTip) : ToolTip(NewToolTip) {}
		virtual MenuTypes GetType() { return Menu_Interactable; }
		virtual void Update() {}
		virtual void Draw()
		{
			auto Pos = GetPosition();
			if (IsCursorInBounds(IntRect{ (int)Pos.x, (int)Pos.y, 160, 10 }))
				ToolTipString = ToolTip;
		}
		virtual void OnInteracted() {}
	protected:
		std::string ToolTip;
	};

	class CCheckbox : public CInteractable
	{
	public:
		CCheckbox(std::string CheckboxLabel, std::string CheckboxToolTip, bool* Value) : CheckboxValue(Value)
		{
			Label = CheckboxLabel;
			ToolTip = CheckboxToolTip;
		}
		virtual MenuTypes GetType() { return Menu_Checkbox; }
		virtual void Draw()
		{
			auto Pos = GetPosition();
			DrawIntRect(IntRect{ (int)Pos.x, (int)Pos.y, 10, 10 }, Color(225, 225, 225, GetOpacity()));
			DrawIntRect(IntRect{ (int)Pos.x + 1, (int)Pos.y + 1, 8, 8 }, Color(48, 48, 48, GetOpacity()));
			if (*CheckboxValue)
				DrawGradientVertRect(IntRect{ (int)Pos.x + 2, (int)Pos.y + 2, 6, 6 }, Color(128, 225, 225, 255), Color(0, 128, 225, GetOpacity()));
			DrawString(Font[FONT_BAR], Vector2D(Pos.x + 16, Pos.y - 1), Color(225, 225, 225, GetOpacity()), 0, Label.c_str());
			CInteractable::Draw();
		}
		virtual void Update()
		{
			auto Pos = GetPosition();
			if (IsCursorInBounds({ (int)Pos.x, (int)Pos.y, 10, 10 }) && IsMouseClicked)
			{
				*CheckboxValue = !*CheckboxValue;
				IsMouseClicked = false;
				OnInteracted();
			}
			CInteractable::Update();
		}
	protected:
		bool* CheckboxValue;
	};

	class CToggleVisibilityCheckbox : public CCheckbox
	{
	public:
		CToggleVisibilityCheckbox(std::string CheckboxLabel, std::string CheckboxToolTip, bool* Value, CBaseRenderable* t) : CCheckbox(CheckboxLabel, CheckboxToolTip, Value)
		{
			Target = t;
		}
		virtual int GetVariant() { return 1; }
		virtual void Initialize()
		{
			OnInteracted();
		}
		virtual void OnInteracted()
		{
			Target->SetVisible(*CheckboxValue);
		}
	protected:
		CBaseRenderable* Target;
	};

	typedef void(*FunctionPtr)();
	class CButton : public CInteractable
	{
	public:
		CButton(std::string ButtonLabel, std::string ButtonToolTip, FunctionPtr Value) : ButtonValue(Value)
		{
			Label = ButtonLabel;
			ToolTip = ButtonToolTip;
		}
		virtual MenuTypes GetType() { return Menu_Button; }
		virtual void Draw()
		{
			auto Pos = GetPosition();
			int w = GetLabelSize().x;
			if (IsBeingDragged)
			{
				DrawIntRect(IntRect{ (int)Pos.x + 1, (int)Pos.y + 1, w + 4, 10 }, Color(96, 96, 96, GetOpacity()));
				DrawIntRect(IntRect{ (int)Pos.x - 1, (int)Pos.y - 1, w + 5, 11 }, Color(32, 32, 32, GetOpacity()));
				DrawIntRect(IntRect{ (int)Pos.x, (int)Pos.y, w + 4, 10 }, Color(48, 48, 48, GetOpacity()));
			}
			else
			{
				DrawIntRect(IntRect{ (int)Pos.x + 1, (int)Pos.y + 1, w + 4, 10 }, Color(32, 32, 32, GetOpacity()));
				DrawIntRect(IntRect{ (int)Pos.x - 1, (int)Pos.y - 1, w + 5, 11 }, Color(96, 96, 96, GetOpacity()));
				DrawIntRect(IntRect{ (int)Pos.x, (int)Pos.y, w + 4, 10 }, Color(64, 64, 64, GetOpacity()));
			}
			DrawString(Font[FONT_BAR], Vector2D(Pos.x + 2, Pos.y - 1), Color(225, 225, 225, GetOpacity()), 0, Label.c_str());
			CInteractable::Draw();
		}
		virtual void Update()
		{
			auto Pos = GetPosition();
			if (IsCursorInBounds({ (int)Pos.x, (int)Pos.y, (int)GetLabelSize().x, 10 }) && IsMouseClicked)
			{
				OnClicked();
				IsBeingDragged = true;
				IsMouseClicked = false;
				OnInteracted();
			}
			if (!IsMouseDown && IsBeingDragged)
				IsBeingDragged = false;
			CInteractable::Update();
		}
		virtual void OnClicked()
		{
			(*ButtonValue)();
		}
	private:
		FunctionPtr ButtonValue;
		bool IsBeingDragged = false;
	};

	class CCombobox : public CInteractable
	{
	public:
		CCombobox(std::string ComboboxLabel, std::string ComboboxToolTip, std::vector<std::string> Options, int* Value) : ComboboxOptions(Options), ComboboxValue(Value)
		{
			Label = ComboboxLabel;
			ToolTip = ComboboxToolTip;
		}
		virtual MenuTypes GetType() { return Menu_Combobox; }
		virtual void Draw()
		{
			auto Pos = GetPosition();
			DrawIntRect(IntRect{ (int)Pos.x, (int)Pos.y, GetDropDownWidth(), GetDropDownSectionHeight() }, Color(96, 96, 96, GetOpacity()));
			DrawIntRect(IntRect{ (int)Pos.x + 1, (int)Pos.y + 1, GetDropDownWidth() - 2, GetDropDownSectionHeight() - 2 }, Color(48, 48, 48, GetOpacity()));
			DrawString(Font[FONT_BAR], Vector2D(Pos.x + 2, Pos.y - 1), Color(225, 225, 225, GetOpacity()), 0, ComboboxOptions[*ComboboxValue].c_str());
			DrawString(Font[FONT_BAR], Vector2D(Pos.x + GetDropDownWidth() + 4, Pos.y - 1), Color(225, 225, 225, GetOpacity()), 0, Label.c_str());

			if (IsOpen)
			{
				// Each section is 10 pixels tall
				int BoxHeight = GetDropDownHeight();
				int BoxWidth = GetDropDownWidth();
				for (int x = 0; x < ComboboxOptions.size(); x++)
				{
					auto CurrentRect = IntRect{ (int)Pos.x, (int)Pos.y + GetDropDownSectionHeight() + GetDropDownSectionHeight() * x, GetDropDownWidth(), GetDropDownSectionHeight() };
					auto Col = IsCursorInBounds(CurrentRect) ? Color(96, 96, 96, GetOpacity()) : Color(32, 32, 32, GetOpacity());
					DrawIntRect(CurrentRect, Col);
				}
				for (int c = 0; c < ComboboxOptions.size(); c++)
					DrawString(Font[FONT_BAR], Vector2D(Pos.x + 2, Pos.y + GetDropDownSectionHeight() - 1 + GetDropDownSectionHeight() * c), Color(225, 225, 225, GetOpacity()), 0, ComboboxOptions[c].c_str());
			}
			if (!IsOpen)
				CInteractable::Draw();
		}
		virtual int GetDropDownSectionHeight() { return 10; }
		virtual int GetDropDownWidth() { return 80; }
		virtual int GetDropDownHeight() { return ComboboxOptions.size() * GetDropDownSectionHeight(); }
		virtual void Update()
		{
			auto Pos = GetPosition();
			if (IsCursorInBounds({ (int)Pos.x, (int)Pos.y, GetDropDownWidth(), GetDropDownSectionHeight() }) && IsMouseClicked && !IsOpen)
			{
				IsOpen = true;
				IsMouseClicked = false;
			}
			if (*ComboboxValue >= ComboboxOptions.size()) *ComboboxValue = 0;
			else if (IsOpen && IsCursorInBounds(IntRect{ (int)Pos.x, (int)Pos.y + GetDropDownSectionHeight(), GetDropDownWidth(), GetDropDownHeight() }) && IsMouseClicked)
			{
				for (int c = 0; c < ComboboxOptions.size(); c++)
				{
					auto CurrentRect = IntRect{ (int)Pos.x, (int)Pos.y + GetDropDownSectionHeight() + GetDropDownSectionHeight() * c, GetDropDownWidth(), GetDropDownSectionHeight() };
					if (IsCursorInBounds(CurrentRect)) { *ComboboxValue = c; break; }
				}
				IsOpen = false;
				IsMouseClicked = false;
				OnInteracted();
			}
			else if (IsOpen && !IsCursorInBounds({ (int)Pos.x, (int)Pos.y, GetDropDownWidth(), GetDropDownHeight() }) && IsMouseClicked)
				IsOpen = false;
			CInteractable::Update();
		}
	private:
		std::vector<std::string> ComboboxOptions;
		bool IsOpen = false;
		int* ComboboxValue;
	};

	class CSlider : public CInteractable
	{
	public:
		CSlider(std::string SliderLabel, std::string SliderToolTip, int MinVal, int MaxVal, int* Value) : MinValue(MinVal), MaxValue(MaxVal), SliderValue(Value)
		{
			Label = SliderLabel;
			ToolTip = SliderToolTip;
		}
		virtual MenuTypes GetType() { return Menu_Slider; }
		virtual void Draw()
		{
			auto Pos = GetPosition();
			int Length = GetParent()->GetBounds().w - 32;
			DrawIntRect(IntRect{ (int)Pos.x, (int)Pos.y, Length, 10 }, Color(96, 96, 96, GetOpacity()));
			DrawIntRect(IntRect{ (int)Pos.x + 1, (int)Pos.y + 1, Length - 2, 8 }, Color(48, 48, 48, GetOpacity()));
			int Width = (float)(Length - 4) * ((float)(*SliderValue - MinValue) / (MaxValue - MinValue));
			DrawGradientVertRect(IntRect{ (int)Pos.x + 2, (int)Pos.y + 2, Width, 6 }, Color(128, 225, 225, GetOpacity()), Color(0, 128, 225, GetOpacity()));
			DrawString(Font[FONT_BAR], Vector2D((int)Pos.x + 2, (int)Pos.y - 1), Color(225, 225, 225, GetOpacity()), 0, Label.c_str());
			int w, h;
			GetTextSize(Font[FONT_BAR], w, h, std::to_string(*SliderValue).substr(0, 4).c_str());
			DrawString(Font[FONT_BAR], Vector2D((int)Pos.x + Length - w, (int)Pos.y - 1), Color(225, 225, 225, GetOpacity()), 0, std::to_string(*SliderValue).substr(0, 4).c_str());
			if (!IsBeingDragged)
				CInteractable::Draw();
		}
		virtual void OnDragged()
		{
			auto Pos = GetPosition();
			int Length = GetParent()->GetBounds().w - 32;
			float MouseXRel = CursorPos.x - Pos.x;
			if (MouseXRel < 0) MouseXRel = 0;
			if (MouseXRel > Length) MouseXRel = Length;
			float DragFrac = (float)MouseXRel / (float)Length;
			*SliderValue = MinValue + DragFrac * (MaxValue - MinValue);
		}
		virtual void Update()
		{
			auto Pos = GetPosition();
			if (IsBeingDragged)
				OnDragged();
			int Length = GetParent()->GetBounds().w - 32;
			if (IsCursorInBounds({ (int)Pos.x, (int)Pos.y, Length, 10 }) && IsMouseClicked)
			{
				IsBeingDragged = true;
				IsMouseClicked = false;
				OnInteracted();
			}
			if (!IsMouseDown && IsBeingDragged)
				IsBeingDragged = false;
			CInteractable::Update();
		}
	private:
		int* SliderValue;
		bool IsBeingDragged = false;
		int MinValue;
		int MaxValue;
	};

	class CSliderF : public CInteractable
	{
	public:
		CSliderF(std::string SliderLabel, std::string SliderToolTip, float MinVal, float MaxVal, float* Value) : MinValue(MinVal), MaxValue(MaxVal), SliderValue(Value)
		{
			Label = SliderLabel;
			ToolTip = SliderToolTip;
		}
		virtual MenuTypes GetType() { return Menu_Slider; }
		virtual int GetVariant() { return 1; }
		virtual void Draw()
		{
			auto Pos = GetPosition();
			int Length = GetParent()->GetBounds().w - 32;
			DrawIntRect(IntRect{ (int)Pos.x, (int)Pos.y, Length, 10 }, Color(96, 96, 96, GetOpacity()));
			DrawIntRect(IntRect{ (int)Pos.x + 1, (int)Pos.y + 1, Length - 2, 8 }, Color(48, 48, 48, GetOpacity()));
			int Width = (float)(Length - 4) * ((float)(*SliderValue - MinValue) / (MaxValue - MinValue));
			DrawGradientVertRect(IntRect{ (int)Pos.x + 2, (int)Pos.y + 2, Width, 6 }, Color(128, 225, 225, GetOpacity()), Color(0, 128, 225, GetOpacity()));
			DrawString(Font[FONT_BAR], Vector2D((int)Pos.x + 2, (int)Pos.y - 1), Color(225, 225, 225, GetOpacity()), 0, Label.c_str());
			int w, h;
			GetTextSize(Font[FONT_BAR], w, h, std::to_string(*SliderValue).substr(0, 4).c_str());
			DrawString(Font[FONT_BAR], Vector2D((int)Pos.x + Length - w, (int)Pos.y - 1), Color(225, 225, 225, GetOpacity()), 0, std::to_string(*SliderValue).substr(0, 4).c_str());
			if (!IsBeingDragged)
				CInteractable::Draw();
		}
		virtual void OnDragged()
		{
			auto Pos = GetPosition();
			int Length = GetParent()->GetBounds().w - 32;
			float MouseXRel = CursorPos.x - Pos.x;
			if (MouseXRel < 0) MouseXRel = 0;
			if (MouseXRel > Length) MouseXRel = Length;
			float DragFrac = (float)MouseXRel / (float)Length;
			*SliderValue = MinValue + DragFrac * (MaxValue - MinValue);
		}
		virtual void Update()
		{
			auto Pos = GetPosition();
			if (IsBeingDragged)
				OnDragged();
			int Length = GetParent()->GetBounds().w - 32;
			if (IsCursorInBounds({ (int)Pos.x, (int)Pos.y, Length, 10 }) && IsMouseClicked)
			{
				IsBeingDragged = true;
				IsMouseClicked = false;
				OnInteracted();
			}
			if (!IsMouseDown && IsBeingDragged)
				IsBeingDragged = false;
			CInteractable::Update();
		}
	private:
		float* SliderValue;
		bool IsBeingDragged = false;
		float MinValue;
		float MaxValue;
	};

	void InitializeMenu()
	{
		InitializeFonts();
		CMainWindow* MainWindow = new CMainWindow(Vector2D(120, 120), Vector2D(480, 360), 14, XorStr("Clean's Pastenz"));
		{
			CTab* AimAssistTab = new CTab(XorStr("Aim Assist/Movement"));
			{
				CVerticalLayout* AimAssist = new CVerticalLayout(XorStr("Aim Assist"));
				{
					AimAssist->AddChild(new CCheckbox(XorStr("Triggerbot"), XorStr("Self-explanatory"), &Config.g_bTriggerbot));
					AimAssist->AddChild(new CCheckbox(XorStr("No Visual Recoil"), XorStr("ez negev"), &Config.g_bNoVisualRecoil));
					AimAssist->AddChild(new CSlider(XorStr("NoRecoil Accuracy"), XorStr("Silver 1 <-> Global Elite"), 0, 100, &Config.g_iRCSAccuracy));
					//AimAssist->AddChild(new CCombobox(XorStr("Aim Assist Mode"), XorStr("Legit aim modes"), std::vector<std::string>{"None", "Aimlock", "Aimbot"}, &Config.g_iAimAssistMode));
					AimAssist->AddChild(new CCombobox(XorStr("Aim Assist Mode"), XorStr("Legit aim modes"), std::vector<std::string>{"None", "Aimlock"}, &Config.g_iAimAssistMode));
					//AimAssist->AddChild(new CCombobox(XorStr("Aimbot Target"), XorStr("Controls legit aim spot, use ALT + H to change"), std::vector<std::string>{XorStr("Head"), XorStr("Chest"), XorStr("Stomach")}, &Config.g_iAimbotTarget));
					AimAssist->AddChild(new CCombobox(XorStr("View Mode"), XorStr("Toggles silent aim"), std::vector<std::string>{XorStr("Normal"), XorStr("Silent"), XorStr("pSilent")}, &Config.g_iViewMode));
				}
				AimAssistTab->AddChild(AimAssist);
				CVerticalLayout* AimbotSettings = new CVerticalLayout(XorStr("Aimbot Settings"));
				{
					//AimAssist->AddChild(new CSliderF(XorStr("Aimbot FOV"), XorStr("Lower = Less aim assist"), 1, 15, &Config.g_fAimbotFOV));
					//AimAssist->AddChild(new CSliderF(XorStr("Aimbot Speed"), XorStr("Measured in degrees per tick"), 1, 5, &Config.g_fAimbotSpeed));
				}
				AimAssistTab->AddChild(AimbotSettings);
			}
			MainWindow->AddChild(AimAssistTab);
			CTab* VisualsTab = new CTab(XorStr("Visuals"));
			{
				CVerticalLayout* ESPColumn = new CVerticalLayout(XorStr("ESP"));
				{
					ESPColumn->AddChild(new CCheckbox(XorStr("Show Boxes"), XorStr("Render 3D ESP boxes"), &Config.g_bESPShowBoxes));
					ESPColumn->AddChild(new CCheckbox(XorStr("Show Skeletons"), XorStr("Draw lines between bones"), &Config.g_bESPShowSkeletons));
					ESPColumn->AddChild(new CCheckbox(XorStr("Show Player Names"), XorStr("Draw name + health"), &Config.g_bESPShowPlayerNames));
					ESPColumn->AddChild(new CCheckbox(XorStr("Show Weapon Names"), XorStr("Draw active weapon name"), &Config.g_bESPShowWeaponNames));
					ESPColumn->AddChild(new CCombobox(XorStr("ESP Quality"), XorStr("Turn down for slightly more FPS"), std::vector<std::string>{XorStr("Fast"), XorStr("Moderate"), XorStr("Fancy")}, &Config.g_iESPQuality));
					ESPColumn->AddChild(new CSlider(XorStr("FOV"), XorStr("Gotta be a Quake pro dude"), 45, 135, &Config.g_iFOV));
					ESPColumn->AddChild(new CSliderF(XorStr("ASUS Opacity"), XorStr("Set transparency of walls"), 0.0f, 1.0f, &Config.g_fASUSOpacity));
				}
				VisualsTab->AddChild(ESPColumn);
				CVerticalLayout* ChamsColumn = new CVerticalLayout(XorStr("Chams"));
				{
					ChamsColumn->AddChild(new CCombobox(XorStr("Player"), XorStr("Pretty colors"), std::vector<std::string>{XorStr("None"), XorStr("Solid"), XorStr("Wireframe")}, &Config.g_iPlayerChams));
					ChamsColumn->AddChild(new CCombobox(XorStr("View Model"), XorStr("More pretty colors"), std::vector<std::string>{XorStr("None"), XorStr("Solid"), XorStr("Wireframe")}, &Config.g_iViewmodelChams));
					ChamsColumn->AddChild(new CCombobox(XorStr("Weapons"), XorStr("Even more pretty colors"), std::vector<std::string>{XorStr("None"), XorStr("Solid"), XorStr("Wireframe")}, &Config.g_iWeaponChams));
					ChamsColumn->AddChild(new CSlider(XorStr("Team Color Red"), XorStr("0 - 255"), 0, 255, &Config.g_iChams_TeamColor[0]));
					ChamsColumn->AddChild(new CSlider(XorStr("Team Color Green"), XorStr("0 - 255"), 0, 255, &Config.g_iChams_TeamColor[1]));
					ChamsColumn->AddChild(new CSlider(XorStr("Team Color Blue"), XorStr("0 - 255"), 0, 255, &Config.g_iChams_TeamColor[2]));
					ChamsColumn->AddChild(new CSliderF(XorStr("Cham Alpha"), XorStr("0.0f - 1.0f"), 0, 1, &Config.g_fChams_Alpha));
				}
				VisualsTab->AddChild(ChamsColumn);
			}
			MainWindow->AddChild(VisualsTab);
			CTab* RagebotTab = new CTab("Ragebot");
			{
				CSubTab* GeneralTab = new CSubTab("General");
				{
					CVerticalLayout* RagebotColumn = new CVerticalLayout("Ragebot");
					{
						RagebotColumn->AddChild(new CCheckbox(XorStr("AutoShoot"), XorStr("Turn it on, no balls"), &Config.g_bAutoShoot));
						RagebotColumn->AddChild(new CCombobox(XorStr("AutoShoot Mode"), XorStr("Bone = Aim at point, Hitscan = Search"), std::vector<std::string>{XorStr("Bone"), XorStr("Hitscan"), XorStr("SkeletonScan")}, &Config.g_iAutoShootMode));
						RagebotColumn->AddChild(new CCombobox(XorStr("AutoShoot Target"), XorStr("Controls rage aim spot (bone mode only), use ALT + H to change"), std::vector<std::string>{XorStr("Head"), XorStr("Chest"), XorStr("Stomach")}, &Config.g_iAutoShootTarget));
						RagebotColumn->AddChild(new CSlider(XorStr("HitChance"), XorStr("Min. chance required for autoshoot to fire"), 0, 100, &Config.g_iMinHitChance));
						RagebotColumn->AddChild(new CSlider(XorStr("Hitscan Resolution"), XorStr("Accuracy at the cost of CPU"), 1, 100, &Config.g_iHitScan_Resolution));
						RagebotColumn->AddChild(new CCheckbox(XorStr("AutoWall"), XorStr("AutoShoot through walls"), &Config.g_bAutoWall));
						RagebotColumn->AddChild(new CSlider(XorStr("AutoWall Min Damage"), XorStr("Minimum damage required for shot"), 1, 100, &Config.g_iAutoWall_MinDamage));
						RagebotColumn->AddChild(new CCombobox(XorStr("FakeLag Mode"), XorStr("Airline to Brazil"), std::vector<std::string>{XorStr("Normal"), XorStr("Step Up"), XorStr("Step Down")}, &Config.g_iFakeLag_Mode));
						RagebotColumn->AddChild(new CSlider(XorStr("FakeLag Factor"), XorStr("Google Fiber <-> Comcast"), 0, 15, &Config.g_iFakeLag_LagFactor));
					}
					GeneralTab->AddChild(RagebotColumn);

					CSplitVerticalLayout* RageBotExtendedColumn = new CSplitVerticalLayout(XorStr("Hitscan Weights"), XorStr("Resolver Options"), 7);
					{
						RageBotExtendedColumn->AddChild(new CSliderF(XorStr("Head"), XorStr("Head priority"), 0, 1, &Config.g_iScanWeights[1]));
						RageBotExtendedColumn->AddChild(new CSliderF(XorStr("Chest"), XorStr("Chest priority"), 0, 1, &Config.g_iScanWeights[2]));
						RageBotExtendedColumn->AddChild(new CSliderF(XorStr("Stomach"), XorStr("Stomach priority"), 0, 1, &Config.g_iScanWeights[3]));
						RageBotExtendedColumn->AddChild(new CSliderF(XorStr("Left Arm"), XorStr("Left Arm priority"), 0, 1, &Config.g_iScanWeights[4]));
						RageBotExtendedColumn->AddChild(new CSliderF(XorStr("Right Arm"), XorStr("Right Arm priority"), 0, 1, &Config.g_iScanWeights[5]));
						RageBotExtendedColumn->AddChild(new CSliderF(XorStr("Left Leg"), XorStr("Left Leg priority"), 0, 1, &Config.g_iScanWeights[6]));
						RageBotExtendedColumn->AddChild(new CSliderF(XorStr("Right Leg"), XorStr("Right Leg priority"), 0, 1, &Config.g_iScanWeights[7]));
					}
					{
						RageBotExtendedColumn->AddChild(new CCheckbox(XorStr("Lower Body Yaw Fix"), XorStr("Try to un-fake angles"), &Config.g_bLowerBodyYawFix));
						RageBotExtendedColumn->AddChild(new CCheckbox(XorStr("Resolve Angles"), XorStr("Try to fix fucked up angles"), &Config.g_bResolve));
						RageBotExtendedColumn->AddChild(new CCombobox(XorStr("Pitch Mode"), XorStr("Pitch search type"), std::vector<std::string>{XorStr("Half Cycle"), XorStr("Full Cycle"), XorStr("Down")}, &Config.g_iResolver_PitchMode));
						RageBotExtendedColumn->AddChild(new CCombobox(XorStr("Yaw Mode"), XorStr("Yaw search type"), std::vector<std::string>{XorStr("Full Cycle"), XorStr("Cross")}, &Config.g_iResolver_YawMode));
						RageBotExtendedColumn->AddChild(new CCombobox(XorStr("Angle Step"), XorStr("Yaw rotation increment"), std::vector<std::string>{XorStr("30 Step"), XorStr("45 Step"), XorStr("90 Step")}, &Config.g_iResolver_AngleStep));
						RageBotExtendedColumn->AddChild(new CCombobox(XorStr("Angle Snap"), XorStr("Apply angles relatively/absolute/to velocity/at target"), std::vector<std::string>{XorStr("Relative"), XorStr("Absolute"), XorStr("Velocity"), XorStr("At Target")}, &Config.g_iResolver_AngleSnapping));
						RageBotExtendedColumn->AddChild(new CCombobox(XorStr("FakeLag Fix"), XorStr("Bring those plebs back up to speed"), std::vector<std::string>{XorStr("None"), XorStr("Wait")}, &Config.g_iFakeLagFix));
					}
					GeneralTab->AddChild(RageBotExtendedColumn);
				}
				RagebotTab->AddChild(GeneralTab);
				CSubTab* AntiAimTab = new CSubTab("Anti-Aim Options");
				{
					CVerticalLayout* AntiAimOptions = new CVerticalLayout("Anti-Aim Options");
					// Create pointer ahead of time so we can pass a pointer to the visibility toggle
					CSplitVerticalLayout* AngleOptions = new CSplitVerticalLayout(XorStr("Real Angles"), XorStr("Fake Angles"), 3);
					// AntiAimOptions Start
					{
						AntiAimOptions->AddChild(new CCombobox(XorStr("Pitch Preset"), XorStr("Vertical anti-aim"), std::vector<std::string>{XorStr("None"), XorStr("Down"), XorStr("Emotion"), XorStr("FakeDown"), XorStr("FakeAngel"), XorStr("Rotary"), XorStr("Rand")}, &Config.g_iAntiAim_Pitch));
						AntiAimOptions->AddChild(new CCombobox(XorStr("Yaw Preset"), XorStr("Horizontal anti-aim"), std::vector<std::string>{XorStr("None"), XorStr("Backward"), XorStr("FakeBackward"), XorStr("Jitter"), XorStr("BackJitter"), XorStr("JitterSynced"), XorStr("Sideways"), XorStr("FakeSideways"), XorStr("FakeForward"), XorStr("SlowSpin"), XorStr("FastSpin"), XorStr("Rand"), XorStr("FakeSideSpin")}, &Config.g_iAntiAim_Yaw));
						AntiAimOptions->AddChild(new CCombobox(XorStr("Yaw Direction"), XorStr("Change anti-aim orientation"), std::vector<std::string>{XorStr("Relative"), XorStr("Absolute"), XorStr("At Target")}, &Config.g_iAntiAim_Direction));
						AntiAimOptions->AddChild(new CToggleVisibilityCheckbox(XorStr("Override Angles"), XorStr("Use custom angles"), &Config.g_bAntiAim_OverrideAngles, AngleOptions));
					}
					AntiAimTab->AddChild(AntiAimOptions);
					// AngleOptions Start
					{
						AngleOptions->AddChild(new CCombobox(XorStr("Anti-Aim Pitch"), XorStr("Vertical anti-aim"), std::vector<std::string>{XorStr("None"), XorStr("Down"), XorStr("Emotion"), XorStr("FakeDown"), XorStr("FakeAngel"), XorStr("Rotary"), XorStr("Rand")}, &Config.g_iCustomAntiAim_PitchOverride));
						AngleOptions->AddChild(new CCombobox(XorStr("Anti-Aim Yaw"), XorStr("Horizontal anti-aim"), std::vector<std::string>{XorStr("None"), XorStr("Backward"), XorStr("BackJitter"), XorStr("JitterSynced"), XorStr("Sideways"), XorStr("SlowSpin"), XorStr("FastSpin"), XorStr("Rand")}, &Config.g_iCustomAntiAim_RealYawOverride));
						AngleOptions->AddChild(new CCombobox(XorStr("Yaw Direction"), XorStr("Change anti-aim orientation"), std::vector<std::string>{XorStr("Relative"), XorStr("Absolute"), XorStr("At Target")}, &Config.g_iCustomAntiAim_RealDirectionOverride));
					}
					{
						AngleOptions->AddChild(new CCheckbox(XorStr("Apply Fake Angle"), XorStr("Mask your real angles"), &Config.g_bCustomAntiAim_ApplyFakeAngle));
						AngleOptions->AddChild(new CCombobox(XorStr("Anti-Aim Yaw"), XorStr("Horizontal anti-aim"), std::vector<std::string>{XorStr("None"), XorStr("Backward"), XorStr("BackJitter"), XorStr("JitterSynced"), XorStr("Sideways"), XorStr("SlowSpin"), XorStr("FastSpin"), XorStr("Rand")}, &Config.g_iCustomAntiAim_FakeYawOverride));
						AngleOptions->AddChild(new CCombobox(XorStr("Yaw Direction"), XorStr("Change anti-aim orientation"), std::vector<std::string>{XorStr("Relative"), XorStr("Absolute"), XorStr("At Target")}, &Config.g_iCustomAntiAim_FakeDirectionOverride));
					}
					AntiAimTab->AddChild(AngleOptions);
				}
				RagebotTab->AddChild(AntiAimTab);
			}
			MainWindow->AddChild(RagebotTab);
			CTab* MiscTab = new CTab(XorStr("Misc"));
			{
				CVerticalLayout* MiscColumn = new CVerticalLayout("Misc");
				{
					MiscColumn->AddChild(new CCheckbox(XorStr("Bunny Hop"), XorStr("Gotta go fast"), &Config.g_bBunnyhop));
					MiscColumn->AddChild(new CCheckbox(XorStr("Autostrafe"), XorStr("Gotta go faster"), &Config.g_bAutoStrafe));
					MiscColumn->AddChild(new CCheckbox(XorStr("Circle Strafe"), XorStr("Gain speed in place (Walk Key)"), &Config.g_bCircleStrafe));
					MiscColumn->AddChild(new CCheckbox(XorStr("Air Stuck"), XorStr("Float in the air (V Key)"), &Config.g_bAirStuck));
					MiscColumn->AddChild(new CButton(XorStr("Aimware Name Change"), XorStr("da bes nospread"), NameChanger::AimwareNameSpam));
					MiscColumn->AddChild(new CButton(XorStr("Interwebz Name Change"), XorStr("da bes baim"), NameChanger::InterwebzNameSpam));
					MiscColumn->AddChild(new CButton(XorStr("No Name"), XorStr("w0w 1337 hax dude"), NameChanger::NoNameSpam));
					MiscColumn->AddChild(new CButton(XorStr("Restore Name"), XorStr("My name is beste name"), NameChanger::RestoreName));
					MiscColumn->AddChild(new CCheckbox(XorStr("Auto Name Stealer"), XorStr("Can't git kicked now.  :^)"), &Config.g_bAutoNameSteal));
					MiscColumn->AddChild(new CCheckbox(XorStr("Auto !rs"), XorStr("Can't go negative now.  :^)"), &Config.g_bAutoResetScore));
					MiscColumn->AddChild(new CCheckbox(XorStr("Enable Untrusted Options"), XorStr("Don't click this in Matchmaking"), &Config.g_bEnableUntrusted));
					MiscColumn->AddChild(new CCombobox(XorStr("Config Slot"), XorStr("Current config number"), std::vector<std::string>{XorStr("Slot 1"), XorStr("Slot 2"), XorStr("Slot 3"), XorStr("Slot 4"), XorStr("Slot 5")}, &ConfigSlot));
					MiscColumn->AddChild(new CButton(XorStr("Load Config"), XorStr("Load config at slot"), LoadConfig));
					MiscColumn->AddChild(new CButton(XorStr("Save Config"), XorStr("Save config at slot"), SaveConfig));
				}
				MiscTab->AddChild(MiscColumn);
			}
			MainWindow->AddChild(MiscTab);
		}
		BaseObjects.push_back(MainWindow);

		for (auto i = BaseObjects.begin(); i != BaseObjects.end(); i++)
			(*i)->Initialize();
	}

	void DrawCursor()
	{
		int x = CursorPos.x;
		int y = CursorPos.y;

		Interfaces::MatSurface()->DrawSetColor(Color(0, 0, 0, MenuAlpha));
		Interfaces::MatSurface()->DrawLine(x, y, x, y + 9);
		Interfaces::MatSurface()->DrawLine(x, y, x + 6, y + 6);
		Interfaces::MatSurface()->DrawLine(x, y + 8, x + 5, y + 6);

		Interfaces::MatSurface()->DrawSetColor(Color(0, 128, 255, MenuAlpha));
		Interfaces::MatSurface()->DrawLine(x + 1, y + 2, x + 1, y + 8);
		Interfaces::MatSurface()->DrawLine(x + 2, y + 3, x + 2, y + 7);
		Interfaces::MatSurface()->DrawLine(x + 3, y + 4, x + 3, y + 7);
		Interfaces::MatSurface()->DrawLine(x + 4, y + 5, x + 4, y + 6);

		if (ToolTipString.size() > 0)
		{
			int w, h;
			GetTextSize(Font[FONT_BAR], w, h, ToolTipString.c_str());
			DrawIntRect(IntRect{ (int)CursorPos.x + 8, (int)CursorPos.y + 8, w + 4, 10 }, Color(96, 96, 96, 255));
			DrawIntRect(IntRect{ (int)CursorPos.x + 9, (int)CursorPos.y + 9, w + 2, 8 }, Color(64, 64, 64, 255));
			DrawString(Font[FONT_BAR], Vector2D(CursorPos.x + 10, CursorPos.y + 7), Color(225, 225, 225, 255), 0, ToolTipString.c_str());
			ToolTipString.clear();
		}
	}

	class Dot;
	std::vector<Dot*> Dots;
	class Dot : public CBaseRenderable
	{
	public:
		Dot(Vector2D p, Vector2D v)
		{
			SetPosition(p, false);
			Velocity = v;
		}
		virtual void Update()
		{
			SetPosition(GetPosition() + Velocity * (MenuAlpha / 255.0f));
			CBaseRenderable::Update();
		}
		virtual void Draw()
		{
			DrawIntRect(IntRect{ (int)GetPosition().x - 2, (int)GetPosition().y - 2, 4, 4 }, Color(225, 225, 225, GetOpacity() / 8.0f));
			auto t = std::find(Dots.begin(), Dots.end(), this);
			if (t == Dots.end()) return;
			for (auto i = t; i != Dots.end(); i++)
			{
				if ((*i) == this) continue;
				Vector2D Pos = GetPosition();
				float Dist = Pos.DistTo((*i)->GetPosition());
				if (Dist < 128)
				{
					Vector2D Dir = ((*i)->GetPosition() - GetPosition()).Normalized();
					Vector2D Right = Vector2D(Dir.y, -Dir.x);
					DrawFilledQuadOneSided(Vector2D(Pos + Right), Vector2D(Pos + Right + Dir * Dist), Vector2D(Pos - Right + Dir * Dist), Vector2D(Pos - Right), Color(255, 255, 255, (MenuAlpha / 16.0f) * (1.0f - (Dist / 128.0f))));
				}
			}
			CBaseRenderable::Draw();
		}
	protected:
		float Rotation = 0;
		float Size = 0;
		Vector2D Velocity;
	};

	void DrawBackdrop()
	{
		int s = rand() % 24;

		if (s == 0)
			Dots.push_back(new Dot(Vector2D(rand() % (int)ScreenSize.x, -16), Vector2D((rand() % 7) - 3, rand() % 3 + 1)));
		else if (s == 1)
			Dots.push_back(new Dot(Vector2D(rand() % (int)ScreenSize.x, (int)ScreenSize.y + 16), Vector2D((rand() % 7) - 3, -1 * (rand() % 3 + 1))));
		else if (s == 2)
			Dots.push_back(new Dot(Vector2D(-16, rand() % (int)ScreenSize.y), Vector2D(rand() % 3 + 1, (rand() % 7) - 3)));
		else if (s == 3)
			Dots.push_back(new Dot(Vector2D((int)ScreenSize.x + 16, rand() % (int)ScreenSize.y), Vector2D(-1 * (rand() % 3 + 1), (rand() % 7) - 3)));

		DrawIntRect(IntRect{ 0, 0, (int)ScreenSize.x, (int)ScreenSize.y }, Color(0, 0, 0, 225 * (MenuAlpha / 255.0f)));

		for (auto i = Dots.begin(); i < Dots.end();)
		{
			if ((*i)->GetPosition().y < - 20 || (*i)->GetPosition().y > ScreenSize.y + 20 || (*i)->GetPosition().x < -20 || (*i)->GetPosition().x > ScreenSize.x + 20)
			{
				delete (*i);
				i = Dots.erase(i);
			}
			else
			{
				(*i)->SetOpacity(MenuAlpha);
				(*i)->Update();
				i++;
			}
		}
		for (auto i = Dots.begin(); i < Dots.end(); i++)
			(*i)->Draw();
	}

	void DestroyBackdrop()
	{
		for (auto i = Dots.begin(); i < Dots.end(); i++)
			delete (*i);
		Dots.clear();
	}

	void Render()
	{
		static DWORD InsertKeyState = false;
		DWORD CurrentInsertKeyState = GetAsyncKeyState(VK_INSERT) < 0;
		if (CurrentInsertKeyState && (CurrentInsertKeyState != InsertKeyState))
		{
			Menu::ShouldDraw = !Menu::ShouldDraw;
			if (!Menu::ShouldDraw)
				Interfaces::Engine()->ExecuteClientCmd(XorStr("cl_mouseenable 1"));
			else
				Interfaces::Engine()->ExecuteClientCmd(XorStr("cl_mouseenable 0"));
		}
		InsertKeyState = CurrentInsertKeyState;

		if (ShouldDraw)
			MenuAlpha = min(MenuAlpha + 6, 255);
		else
			MenuAlpha = max(MenuAlpha - 6, 0);

		if (MenuAlpha)
		{
			for (auto i = BaseObjects.begin(); i != BaseObjects.end(); i++)
				(*i)->SetOpacity(MenuAlpha);

			int ScrW, ScrH, MouseX, MouseY;
			Interfaces::MatSurface()->GetScreenSize(ScrW, ScrH);
			Interfaces::MatSurface()->SurfaceGetCursorPos(MouseX, MouseY);
			ScreenSize = Vector2D(ScrW, ScrH);
			CursorDeltaPos = Vector2D(MouseX, MouseY) - CursorPos;
			CursorPos = Vector2D(MouseX, MouseY);

			// Mouse Input
			static DWORD MouseState = false;
			DWORD CurrentMouseInput = GetAsyncKeyState(VK_LBUTTON) < 0;
			if (CurrentMouseInput && (CurrentMouseInput != MouseState))
				IsMouseClicked = true;
			else
				IsMouseClicked = false;
			MouseState = CurrentMouseInput;
			IsMouseDown = MouseState;

			DrawBackdrop();

			// Reverse iteration update cause reasons.
			for (int i = BaseObjects.size() - 1; i >= 0; i--)
			{
				if (BaseObjects[i]->GetVisible())
					BaseObjects[i]->Update();
			}

			for (int i = 0; i < BaseObjects.size(); i++)
			{
				if (BaseObjects[i]->GetVisible())
					BaseObjects[i]->Draw();
			}

			DrawCursor();
		}
	}

	void DestroyObjects()
	{
		for (auto i = BaseObjects.begin(); i != BaseObjects.end(); i++)
			delete (*i);
		BaseObjects.clear();

		DestroyBackdrop();
	}
}
