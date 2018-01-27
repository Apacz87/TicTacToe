#pragma once

#include <array>
#include <exception>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>
#include "GameSettings.h"

namespace TicTacGame
{
	// Forward declarations classes included in this code module:
	class GameBoard;
	class Game;
	class GameNode;

	// The Player.
	enum Player
	{
		NONE = '-',
		CROSS = 'X',
		CIRCLE = 'O'
	};

	// The GameBoard Class
	class GameBoard
	{
	private:
		// Game board fields
		std::array<Player, 9> m_fields;

	public:
		// The GameBoard class constructor.
		GameBoard();

		// Return true if the indicated player won.
		inline bool checkIfPlayerWon(const Player&) const;

		// Return true if game board is full.
		inline bool isBoardFull() const;

		// Return number of occupied fields in board game.
		inline int numberOfOccupiedFields() const;

		// Returns size of game board.
		inline std::size_t size() const;

		// Return true if the indicated field is free.
		bool isFieldFree(const int&) const;

		// Checking if game is over, returns the winner.
		Player winner() const;

		// GameBoard subscript operator.
		Player& operator[](std::size_t idx);

		// GameBoard subscript operator.
		const Player& operator[](std::size_t idx) const;

	};

	// The GameNode Class.
	class GameNode : public std::enable_shared_from_this<GameNode>
	{
	private:
		// Friend declaration
		friend class Game;

		// The number of existing nodes.
		static int m_numberOfNodes;

		// The last move value.
		short m_lastMove;

		// The Game board current state.
		GameBoard m_gameState;

		// The parent node.
		std::weak_ptr<GameNode> m_parentNode;

		// The derived nodes.
		std::vector<std::shared_ptr<GameNode>> m_derivedNodes;

		// The node mutex.
		std::mutex m_nodeMutex;

		// The current player.
		Player m_currentPlayer;

		// Add node to derived nodes list of a current node.
		void addChildNode(GameBoard, const int&);

		// Recursive generating child nodes in game tree.
		void recursiveTreeWalk(std::shared_ptr<GameNode>);

		// Returns distance from root for current node.
		inline int distanceFromRoot() const;


	public:
		// The GameNode class constructor.
		GameNode(Player);

		// The GameNode class constructor.
		GameNode(std::shared_ptr<GameNode>, Player, GameBoard, short);

		// The GameNode class destructor.
		~GameNode();

		// Returns True if the node is a leaf.
		inline bool leaf() const;

		// Return number of existing nodes.
		inline int totalNumberOfNodes() const;

		// Return value of base move for current node.
		inline short baseMove() const;

		// Return value of game tree node.
		int nodeVale() const;

		// Delete all derived nodes of a current node.
		void deleteChildNodes();

		// Generate child nodes for current node.
		void generateChildNodes();

	};

	// The Game Class
	class Game
	{
	private:
		// The algorithm implementation selected by user.
		Algorithm selectedAlgorithm;

		// The current player.
		Player player;

		// Determines whether if AI is playing.
		bool playWithAI;

		// The game board.
		GameBoard board;

		// Thread generating the game tree.
		std::future<std::shared_ptr<GameNode>> threadGeneratingGameTree;

		// The available movements in current game state.
		std::map<short, int> availableMovements;

		// The game tree root node.
		std::shared_ptr<GameNode> rootNode;

		// Delete the old Nodes.
		void DeleteOldNodes(std::shared_ptr<GameNode>);

		// Update available movements container.
		void UpdateAvailableMovements();

		// Update the root node in tree.
		void UpdateRootNode(const short&);

		// Delete unreachable game tree nodes.
		void CleanUpTree();

		// Generates new game tree.
		std::shared_ptr<GameNode> generateTree();

		// The score of game state.
		int MinMaxScore(const GameBoard&, const int&) const;

		// Returns the value of possible game state.
		int MinMax(GameBoard, Player, int) const;

		// Returns the number of the best available move for the current player.
		int MinMaxBestMove() const;

	public:
		// The Game class constructor.
		Game(const GameSettings&);

		// The Game class destructor.
		~Game();

		// Make move in game board.
		void MakeMove(const int&);

		// Returns True if move is allowed
		bool MoveIsAllowed(const int&) const;

		// Returns True if game is over.
		bool IsGameOver() const;

		// Returns True if AI is Playing.
		bool AiIsPlaying() const;

		// Returns True if specified player won.
		bool CheckIfSpecifiedPlayerWon(const Player&) const;

		// Returns current Player.
		Player CurrentPlayer() const;

		// Returns number of existing nodes in game tree.
		int NumberOfExistingNodes() const;

		// Switch current player.
		void SwitchPlayer();

		// Returns the number of the best available move for the current player.
		int BestAvailableMove();

		// Returns the number of the best available move for the indicated player.
		int BestAvailableMove(const Player&) const;

	};
}
