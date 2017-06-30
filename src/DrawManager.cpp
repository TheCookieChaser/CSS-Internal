#include "DrawManager.h"

std::unique_ptr<CDrawManager> DrawManager = nullptr;

CDrawManager::CDrawManager(IDirect3DDevice9* vDevice)
{
	_device = vDevice;
	_texture = nullptr;
	_drawList = nullptr;
}

CDrawManager::~CDrawManager()
{

}

bool CDrawManager::ScreenTransform(const Vector& point, D3DXVECTOR2& screen)
{
	//https://www.unknowncheats.me/forum/847878-post1.html
	const D3DMATRIX& w2sMatrix = *(D3DMATRIX*)((DWORD)GetModuleHandleA("engine.dll") + 0x5B3D70);
	screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
	screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];

	float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

	if (w < 0.001f)
	{
		screen.x *= 100000;
		screen.y *= 100000;
		return true;
	}

	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	return false;
}

bool CDrawManager::WorldToScreen(const Vector& origin, D3DXVECTOR2& screen)
{
	if (!ScreenTransform(origin, screen))
	{
		int iScreenWidth, iScreenHeight;
		engine->GetScreenSize(iScreenWidth, iScreenHeight);

		screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
		screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;

		return true;
	}
	return false;
}

bool get_system_font_path(const std::string& name, std::string& path)
{
	//
	// This code is not as safe as it should be.
	// Assumptions we make:
	//  -> GetWindowsDirectoryA does not fail.
	//  -> The registry key exists.
	//  -> The subkeys are ordered alphabetically
	//  -> The subkeys name and data are no longer than 260 (MAX_PATH) chars.
	//

	char buffer[MAX_PATH];
	HKEY registryKey;

	GetWindowsDirectoryA(buffer, MAX_PATH);
	std::string fontsFolder = buffer + std::string("\\Fonts\\");

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &registryKey)) {
		return false;
	}

	uint32_t valueIndex = 0;
	char valueName[MAX_PATH];
	uint8_t valueData[MAX_PATH];
	std::wstring wsFontFile;

	do {
		uint32_t valueNameSize = MAX_PATH;
		uint32_t valueDataSize = MAX_PATH;
		uint32_t valueType;

		auto error = RegEnumValueA(
			registryKey,
			valueIndex,
			valueName,
			reinterpret_cast<DWORD*>(&valueNameSize),
			0,
			reinterpret_cast<DWORD*>(&valueType),
			valueData,
			reinterpret_cast<DWORD*>(&valueDataSize));

		valueIndex++;

		if (error == ERROR_NO_MORE_ITEMS) {
			RegCloseKey(registryKey);
			return false;
		}

		if (error || valueType != REG_SZ) {
			continue;
		}

		if (_strnicmp(name.data(), valueName, name.size()) == 0) {
			path = fontsFolder + std::string((char*)valueData, valueDataSize);
			RegCloseKey(registryKey);
			return true;
		}
	} while (true);

	return false;
}

void CDrawManager::CreateObjects()
{
	_drawList = new ImDrawList();

	auto font_path = std::string{};

	uint8_t* pixel_data;

	int width,
		height,
		bytes_per_pixel;

	if (!get_system_font_path(FontName, font_path)) return;

	auto font = _fonts.AddFontFromFileTTF(font_path.data(), FontSize, 0, _fonts.GetGlyphRangesDefault());

	_fonts.GetTexDataAsRGBA32(&pixel_data, &width, &height, &bytes_per_pixel);

	auto hr = _device->CreateTexture(
		width, height,
		1,
		D3DUSAGE_DYNAMIC,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&_texture,
		NULL);

	if (FAILED(hr)) return;

	D3DLOCKED_RECT tex_locked_rect;
	if (_texture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
		return;
	for (int y = 0; y < height; y++)
		memcpy((uint8_t*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixel_data + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
	_texture->UnlockRect(0);

	_fonts.TexID = _texture;
}

void CDrawManager::InvalidateObjects()
{
	if (_texture) _texture->Release();
	_texture = nullptr;

	_fonts.Clear();

	if (_drawList)
		delete _drawList;
	_drawList = nullptr;
}

void CDrawManager::BeginRendering()
{
	_drawData.Valid = false;

	_drawList->Clear();
	_drawList->PushClipRectFullScreen();
}

void CDrawManager::EndRendering()
{
	ImGui_ImplDX9_RenderDrawLists(GetDrawData());
}

void CDrawManager::DrawText(ImVec2 point, ImU32 color, text_flags flags, const char* text, ...)
{
	static const auto MAX_BUFFER_SIZE = 1024;
	static char buffer[MAX_BUFFER_SIZE] = "";

	auto font = _fonts.Fonts[0];

	_drawList->PushTextureID(_fonts.TexID);

	va_list va;
	va_start(va, text);
	vsnprintf_s(buffer, MAX_BUFFER_SIZE, text, va);
	va_end(va);

	if (flags & centered_x || flags & centered_y) {
		auto text_size = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, buffer);
		if (flags & centered_x)
			point.x -= text_size.x / 2;
		if (flags & centered_y)
			point.y -= text_size.y / 2;
	}

	if (flags & outline) {
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x - 1, point.y - 1 }, 0xFF000000, buffer);
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x + 1, point.y }, 0xFF000000, buffer);
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x    , point.y + 1 }, 0xFF000000, buffer);
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x - 1, point.y }, 0xFF000000, buffer);
	}

	if (flags & drop_shadow && !(flags & outline)) {
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x + 1, point.y + 1 }, 0xFF000000, buffer);
	}

	_drawList->AddText(font, font->FontSize, point, color, buffer);
	_drawList->PopTextureID();
}

void CDrawManager::DrawRect(const ImVec2 & a, const ImVec2 & b, ImU32 col, float rounding, int rounding_corners_flags, float thickness)
{
	_drawList->AddRect(a, b, col, rounding, rounding_corners_flags, thickness);
}

void CDrawManager::DrawRectFilled(const ImVec2 & a, const ImVec2 & b, ImU32 col, float rounding, int rounding_corners_flags)
{
	_drawList->AddRectFilled(a, b, col, rounding, rounding_corners_flags);
}

void CDrawManager::DrawLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness)
{
	_drawList->AddLine(a, b, col, thickness);
}

ImDrawData* CDrawManager::GetDrawData()
{
	if (!_drawList->VtxBuffer.empty()) {
		_drawData.Valid = true;
		_drawData.CmdLists = &_drawList;
		_drawData.CmdListsCount = 1;
		_drawData.TotalVtxCount = _drawList->VtxBuffer.Size;
		_drawData.TotalIdxCount = _drawList->IdxBuffer.Size;
	}
	return &_drawData;
}