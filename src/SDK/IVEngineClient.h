#pragma once

class IVEngineClient
{
public:
	void GetScreenSize(int& width, int& height)
	{
		typedef void(__thiscall* GetScreenSizeFn)(void*, int&, int&);
		return CallVFunction<GetScreenSizeFn>(this, 5)(this, width, height);
	}

	bool GetPlayerInfo(int index, player_info_t* pInfo)
	{
		typedef bool(__thiscall* GetPlayerInfoFn)(void*, int, player_info_t*);
		return CallVFunction<GetPlayerInfoFn>(this, 8)(this, index, pInfo);
	}

	int GetLocalPlayer()
	{
		typedef int(__thiscall* GetLocalPlayerFn)(void*);
		return CallVFunction<GetLocalPlayerFn>(this, 12)(this);
	}

	float Time()
	{
		typedef float(__thiscall* TimeFn)(void*);
		return CallVFunction<TimeFn>(this, 14)(this);
	}

	void GetViewAngles(Vector &ang)
	{
		typedef void(__thiscall* GetViewAnglesFn)(void*, Vector&);
		return CallVFunction<GetViewAnglesFn>(this, 19)(this, ang);
	}

	void SetViewAngles(Vector &ang)
	{
		typedef void(__thiscall* SetViewAnglesFn)(void*, Vector&);
		return CallVFunction<SetViewAnglesFn>(this, 20)(this, ang);
	}

	int GetMaxClients()
	{
		typedef int(__thiscall* GetMaxClientsFn)(void*);
		return CallVFunction<GetMaxClientsFn>(this, 21)(this);
	}

	bool IsInGame()
	{
		typedef bool(__thiscall* IsInGameFn)(void*);
		return CallVFunction<IsInGameFn>(this, 26)(this);
	}

	bool IsConnected()
	{
		typedef bool(__thiscall* IsConnectedFn)(void*);
		return CallVFunction<IsConnectedFn>(this, 27)(this);
	}

	bool IsTakingScreenshot()
	{
		typedef bool(__thiscall* IsTakingScreenshotFn)(void*);
		return CallVFunction<IsTakingScreenshotFn>(this, 85)(this);
	}

	void ClientCmd_Unrestricted(const char* command)
	{
		typedef bool(__thiscall* IsTakingScreenshotFn)(void*, const char*);
		CallVFunction<IsTakingScreenshotFn>(this, 106)(this, command);
	}
};