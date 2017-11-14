#pragma once

#include <array>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

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
		// Friend declaration
		friend class GameNode;
		friend class Game;

		// Game board fields
		std::array<Player, 9> fields;

		// Return number of occupied fields in board game.
		int NumberOfOccupiedFields();

		// Checking if game is over, someone won.
		int Win();

		// Sets the value on the indicated field.
		void SetField(int, char);

		// Return true if the indicated field is free.
		bool IsFieldFree(int);

		// Return true if the indicated player won.
		bool CheckIfPlayerWon(Player);

		// Return true if game board is full.
		bool IsBoardFull();

	public:
		// The GameBoard class constructor.
		GameBoard();
	};

	// The GameNode Class.
	class GameNode : public std::enable_shared_from_this<GameNode>
	{
	private:
		// Friend declaration
		friend class Game;

		// The number of existing nodes.
		static int numberOfNodes;

		// The last move value.
		short lastMove;

		// The Game board current state.
		GameBoard gameState;

		// The parent node.
		std::weak_ptr<GameNode> parentNode;

		// The derived nodes.
		std::vector<std::shared_ptr<GameNode>> derivedNodes;

		// The node mutex.
		std::mutex nodeMutex;

		// The current player.
		Player currentPlayer;

		// Add node to derived nodes list of a current node.
		void AddChildNode(GameBoard, int);

		// Recursive generating child nodes in game tree.
		void RecursiveTreeWalk(std::shared_ptr<GameNode>);

		// Returns distance from root for current node.
		int DistanceFromRoot();


	public:
		// The GameNode class constructor.
		GameNode(Player);

		// The GameNode class constructor.
		GameNode(std::shared_ptr<GameNode>, Player, GameBoard, short);

		// The GameNode class destructor.
		~GameNode();

		// Delete all derived nodes of a current node.
		void DeleteChildNodes();

		// Generate child nodes for current node.
		void GenerateChildNodes();

		// Returns True if the node is a leaf.
		bool Leaf();

		// Return number of existing nodes.
		int TotalNumberOfNodes();

		// Return value of game tree node.
		int NodeVale();

		// Return value of base move for current node.
		short BaseMove();

	};

	// The Game Class
	class Game
	{
	private:
		// The current player.
		Player player;

		// Determines whether if AI is playing.
		bool playWithAI;

		// The game board.
		GameBoard board;

		// The game tree.
		std::shared_ptr<GameNode> gameTree;

		// Thread generating the game tree.
		std::future<std::shared_ptr<GameNode>> threadGeneratingGameTree;

		// The available movements in current game state.
		std::map<short, int> availableMovements;

		// Delete the old Nodes.
		void DeleteOldNodes(std::shared_ptr<GameNode>);

		// Update available movements container.
		void UpdateAvailableMovements();

		// Generates new game tree.
		std::shared_ptr<GameNode> generateTree();
	public:
		// The Game class constructor.
		Game(bool);

		// Make move in game board and returns true if succeed.
		bool MakeMove(int);

		// Returns True if game is over.
		bool IsGameOver();

		// Returns True if AI is Playing.
		bool AiIsPlaying();

		// Returns True if specified player won.
		bool CheckIfSpecifiedPlayerWon(Player);

		// Returns current Player.
		Player CurrentPlayer();

		// The gae tree root node.
		std::shared_ptr<GameNode> rootNode;

		// Returns number of existing nodes in game tree.
		int NumberOfExistingNodes();

		// Update the root node in tree.
		void UpdateRootNode(short);

		// Delete unreachable game tree nodes.
		void CleanUpTree();

		// Switch current player.
		void SwitchPlayer();

		// Returns the number of the best available move for the current player.
		int BestAvailableMove();

		// Returns the number of the best available move for the indicated player.
		int BestAvailableMove(Player);
	};
}