#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

namespace TicTacGame
{
	// The AI Algorithm implementation.
	enum Algorithm
	{
		GAMETREE,
		RECURSION
	};

	// The Game settings
	struct GameSettings
	{
		// True if player wants to play against  the AI.
		bool AI;

		// The selected AI implementation.
		Algorithm Implementation;
	};
}

#endif /* GAME_SETTINGS_H */