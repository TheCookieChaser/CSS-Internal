#pragma once

#include "Hooks.h"

#ifdef DrawText
#undef DrawText
#endif

enum text_flags
{
	centered_x = 1,
	centered_y = 2,
	outline = 4,
	drop_shadow = 8
};

class CDrawManager
{
public:
	CDrawManager(IDirect3DDevice9* vDevice);
	~CDrawManager();

	bool ScreenTransform(const Vector & point, D3DXVECTOR2 & screen);
	bool WorldToScreen(const Vector & origin, D3DXVECTOR2 & screen);

	void CreateObjects();
	void InvalidateObjects();
	void BeginRendering();
	void EndRendering();

	void DrawText(ImVec2 point, ImU32 color, text_flags flags, const char* text, ...);
	void DrawRect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ~0, float thickness = 1.0f);
	void DrawRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ~0);
	void DrawLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f);

private:
	ImDrawData* GetDrawData();

	IDirect3DDevice9*   _device;
	IDirect3DTexture9*  _texture;
	ImDrawList*         _drawList;
	ImDrawData          _drawData;
	ImFontAtlas         _fonts;

	const std::string   FontName = "Tahoma";
	const float         FontSize = 14.0f;
};

extern std::unique_ptr<CDrawManager> DrawManager;

//#ifndef DrawText
//#define DrawText DrawTextA
//#endif