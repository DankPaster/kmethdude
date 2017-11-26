#pragma once

#include "Definitions.hpp"
#include "Vector.hpp"
#include "QAngle.hpp"

namespace SourceEngine
{
	struct MoveData { byte Data[256]; };

	class IGameMovement
	{
	public:
		virtual			~IGameMovement(void) {}

		virtual void	ProcessMovement(SourceEngine::IClientEntity *pPlayer, MoveData *pMove) = 0;
		virtual void	Reset(void) = 0;
		virtual void	StartTrackPredictionErrors(SourceEngine::IClientEntity *pPlayer) = 0;
		virtual void	FinishTrackPredictionErrors(SourceEngine::IClientEntity *pPlayer) = 0;
		virtual void	DiffPrint(char const *fmt, ...) = 0;

		virtual SourceEngine::Vector const&	GetPlayerMins(bool ducked) const = 0;
		virtual SourceEngine::Vector const&	GetPlayerMaxs(bool ducked) const = 0;
		virtual SourceEngine::Vector const& GetPlayerViewOffset(bool ducked) const = 0;

		virtual bool			IsMovingPlayerStuck(void) const = 0;
		virtual C_BaseEntity*	GetMovingPlayer(void) const = 0;
		virtual void			UnblockPusher(SourceEngine::IClientEntity* pPlayer, SourceEngine::IClientEntity *pPusher) = 0;

		virtual void SetupMovementBounds(MoveData *pMove) = 0;
	};
};