#pragma once

#include "Vector.h"
#include "imovehelper.h"

class IMoveHelper;
class CBasePlayer;

class CMoveData
{
public:
	char pad[168];
};

class IGameMovement
{
public:
	virtual			~IGameMovement(void) {}

	// Process the current movement command
	virtual void	ProcessMovement(IClientEntity *pPlayer, CMoveData *pMove) = 0;
	virtual void	StartTrackPredictionErrors(IClientEntity *pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(IClientEntity *pPlayer) = 0;
	virtual void	DiffPrint(char const *fmt, ...) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual Vector	GetPlayerMins(bool ducked) const = 0;
	virtual Vector	GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector  GetPlayerViewOffset(bool ducked) const = 0;

};