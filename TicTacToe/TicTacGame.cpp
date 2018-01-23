#include "stdafx.h"
#include "TicTacGame.h"
#include <thread>

namespace TicTacGame
{
	// Number of existing game tree nodes.
	int GameNode::numberOfNodes = 0;

	// The GameBoard class constructor.
	GameBoard::GameBoard()
	{
		fields.fill(Player::NONE);
	}

	// Checking if game is over, returns the winner.
	Player GameBoard::Winner() const
	{
		// Auxiliary variables
		bool firstLine, secondLine, thirdLine, firstColumn, secondColumn, thirdColumn, firstDiagonal, secondDiagonal;

		// Checking for success in the line:
		firstLine = ((this->fields[0] == this->fields[1]) && (this->fields[1] == this->fields[2]) && (this->fields[0] != Player::NONE));
		secondLine = ((this->fields[3] == this->fields[4]) && (this->fields[4] == this->fields[5]) && (this->fields[3] != Player::NONE));
		thirdLine = ((this->fields[6] == this->fields[7]) && (this->fields[7] == this->fields[8]) && (this->fields[6] != Player::NONE));

		// Checking success in the column:
		firstColumn = ((this->fields[0] == this->fields[3]) && (this->fields[3] == this->fields[6]) && (this->fields[0] != Player::NONE));
		secondColumn = ((this->fields[1] == this->fields[4]) && (this->fields[4] == this->fields[7]) && (this->fields[1] != Player::NONE));
		thirdColumn = ((this->fields[2] == this->fields[5]) && (this->fields[5] == this->fields[8]) && (this->fields[2] != Player::NONE));

		// Checking success diagonally:
		firstDiagonal = ((this->fields[0] == this->fields[4]) && (this->fields[4] == this->fields[8]) && (this->fields[0] != Player::NONE));
		secondDiagonal = ((this->fields[2] == this->fields[4]) && (this->fields[4] == this->fields[6]) && (this->fields[2] != Player::NONE));

		if (!(firstLine || secondLine || thirdLine || firstColumn || secondColumn || thirdColumn || firstDiagonal || secondDiagonal))
			return Player::NONE; // No winner

		// Checking who won:
		if (firstLine) return this->fields[1];
		if (secondLine) return this->fields[3];
		if (thirdLine) return this->fields[6];
		if (firstColumn) return this->fields[3];
		if (secondColumn) return this->fields[4];
		if (thirdColumn) return this->fields[5];
		if (firstDiagonal) return this->fields[4];
		// secondDiagonal varible must be TRUE.
		return this->fields[4];
	}

	// Check that the indicated field is free.
	bool GameBoard::IsFieldFree(const int& nr) const
	{
		if (nr<0 || nr>8) return false;
		return fields[nr] == Player::NONE;
	}

	// Sets the value on the indicated field.
	void GameBoard::SetField(const int& nr, const char& player)
	{
		if (nr<0 || nr>8) std::out_of_range("Field value is out of reange!");
		if (player == 'X') fields[nr] = Player::CROSS;
		else fields[nr] = Player::CIRCLE;
	}

	// Check whether the indicated player won.
	bool GameBoard::CheckIfPlayerWon(const Player& player) const
	{
		return this->Winner() == player;
	}

	// Check if game board is full.
	bool GameBoard::IsBoardFull() const
	{
		return this->fields.end() == std::find(this->fields.begin(), this->fields.end(), Player::NONE);
	}

	// Return number of occupied fields in board game.
	int GameBoard::NumberOfOccupiedFields() const
	{
		return std::count_if(this->fields.begin(), this->fields.end(), [&](Player ply){ return ply != Player::NONE; });
	}

	// The GameNode class constructor.
	GameNode::GameNode(Player ply = Player::CROSS) : gameState(), lastMove(-1), currentPlayer(ply)
	{
		this->numberOfNodes++;
	}

	// The GameNode class constructor.
	GameNode::GameNode(std::shared_ptr<GameNode> base, Player ply, GameBoard board, short move) :
		gameState(board), lastMove(move), parentNode(base), currentPlayer(ply)
	{
		this->numberOfNodes++;
	}

	// The GameNode class destructor.
	GameNode::~GameNode()
	{
		//std::lock_guard<std::mutex> lg(this->nodeMutex);
		this->DeleteChildNodes();
		this->numberOfNodes--;
	}

	// Add node to derived nodes list of a current node.
	void GameNode::AddChildNode(GameBoard board, const int& field)
	{
		board.SetField(field, this->currentPlayer);
		auto nextPlayer = this->currentPlayer == Player::CROSS ? Player::CIRCLE : Player::CROSS;
		this->derivedNodes.push_back(std::make_shared<GameNode>(shared_from_this(), nextPlayer, board, field));
	}

	// Generate child nodes for current node.
	void GameNode::GenerateChildNodes()
	{
		if (!this->Leaf())
		{
			std::lock_guard<std::mutex> lg(this->nodeMutex);
			for (int i = 0; i < 9; i++)
			{
				if (this->gameState.IsFieldFree(i))
				{
					this->AddChildNode(this->gameState, i);
				}
			}
		}
	}

	// Delete all derived nodes of a current node.
	void GameNode::DeleteChildNodes()
	{
		std::lock_guard<std::mutex> lg(this->nodeMutex);
		this->derivedNodes.clear();
	}

	// Return number of existing nodes.
	int GameNode::TotalNumberOfNodes() const
	{
		return this->numberOfNodes;
	}

	// Returns True if the node is a leaf.
	bool GameNode::Leaf() const
	{
		return (this->gameState.Winner() != Player::NONE) || this->gameState.IsBoardFull();
	}

	// Recursive generating child nodes in game tree.
	void GameNode::RecursiveTreeWalk(std::shared_ptr<GameNode> partialTree)
	{
		if (!partialTree->Leaf())
		{
			if (partialTree->derivedNodes.empty())
			{
				partialTree->GenerateChildNodes();
			}

			for each (auto node in partialTree->derivedNodes)
			{
				node->RecursiveTreeWalk(node);
			}
		}
	}

	// Return value of game tree node.
	int GameNode::NodeVale() const
	{
		if (this->Leaf())
		{
			if (this->gameState.CheckIfPlayerWon(Player::CROSS))
			{
				auto tmp = 10 - this->DistanceFromRoot();
				return tmp;
			}
			else if (this->gameState.CheckIfPlayerWon(Player::CIRCLE))
			{
				auto tmp = this->DistanceFromRoot() - 10;
				return tmp;
			}

			return 0;
		}

		auto sum_of_nodes_value = 0;
		if (!this->derivedNodes.empty())
		{
			std::for_each(this->derivedNodes.begin(), this->derivedNodes.end(), [&](std::shared_ptr<GameNode> n) {sum_of_nodes_value += n->NodeVale(); });
		}

		return sum_of_nodes_value;
	}

	// Returns value of base move.
	short GameNode::BaseMove() const
	{
		return this->lastMove;
	}

	// Returns distance from root node.
	int GameNode::DistanceFromRoot() const
	{
		if (!this->parentNode.expired())
		{
			return 1 + this->parentNode.lock()->DistanceFromRoot();
		}

		return 0;
	}

	// The Game class constructor.
	Game::Game(const GameSettings& settings) : availableMovements(), playWithAI(settings.AI), selectedAlgorithm(settings.Implementation), player(Player::CROSS)
	{
		if (this->playWithAI && (this->selectedAlgorithm == Algorithm::GAMETREE))
		{
			this->threadGeneratingGameTree = std::async(&Game::generateTree, this);
		}
	}

	// Returns True if AI is Playing.
	bool Game::AiIsPlaying() const
	{
		return this->playWithAI;
	}

	// Returns True if specified player won.
	bool Game::CheckIfSpecifiedPlayerWon(const Player& ply) const
	{
		return this->board.CheckIfPlayerWon(ply);
	}

	// Returns number of existing nodes in game tree.
	int Game::NumberOfExistingNodes() const
	{
		return gameTree->TotalNumberOfNodes();
	}

	// Switch current player.
	void Game::SwitchPlayer(){ this->player = player == Player::CROSS ? Player::CIRCLE : Player::CROSS; }

	// Returns current Player.
	Player Game::CurrentPlayer() const
	{
		return this->player;
	}

	// Generate game tree.
	std::shared_ptr<GameNode> Game::generateTree()
	{
		auto newGameTreeRoot = std::make_shared<GameNode>(Player::CROSS);
		newGameTreeRoot->RecursiveTreeWalk(newGameTreeRoot);
		return newGameTreeRoot;
	}

	// Update available movements list.
	void Game::UpdateAvailableMovements()
	{
		this->availableMovements.clear();
		for each (auto node in this->rootNode->derivedNodes)
		{
			this->availableMovements.emplace(std::make_pair(node->BaseMove(), node->NodeVale()));
		}
	}

	// Update the root node in tree.
	void Game::UpdateRootNode(const short& move)
	{
		if (this->rootNode == nullptr)
		{
			this->gameTree = this->threadGeneratingGameTree.get();
			this->rootNode = this->gameTree;
		}

		auto selectedNode = std::find_if(this->rootNode->derivedNodes.begin(), this->rootNode->derivedNodes.end(), [move](std::shared_ptr<GameNode> node){ return node->BaseMove() == move; });
		this->rootNode = selectedNode->get()->shared_from_this();
		this->UpdateAvailableMovements();
	}

	// Delete the old Nodes.
	void Game::DeleteOldNodes(std::shared_ptr<GameNode> oldRoot)
	{
		oldRoot->DeleteChildNodes();
	}

	// Delete unreachable game tree nodes.
	void Game::CleanUpTree()
	{
		if (this->gameTree != this->rootNode)
		{
			std::thread cleanUpThread(&Game::DeleteOldNodes, this, this->gameTree);
			cleanUpThread.detach();
			this->gameTree = this->rootNode;
		}
	}

	// Returns the number of the best available move for the indicated player.
	int Game::BestAvailableMove(const Player& player) const
	{
		if (this->selectedAlgorithm == Algorithm::GAMETREE)
		{
			if (player == Player::CROSS)
			{
				return std::max_element(this->availableMovements.begin(), this->availableMovements.end(),
					[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
					return p1.second < p2.second; })->first;
			}

			return std::min_element(this->availableMovements.begin(), this->availableMovements.end(),
				[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
				return p1.second < p2.second; })->first;
		}
		else
		{
			return this->MinMaxBestMove();
		}
	}

	// Returns the number of the best available move for the current player.
	int Game::BestAvailableMove()
	{
		return this->BestAvailableMove(this->CurrentPlayer());
	}

	// Make move in game board and returns true if succeed.
	bool Game::MakeMove(const int& f)
	{
		if (this->board.IsFieldFree(f))
		{
			this->board.SetField(f, this->CurrentPlayer());
			if (this->AiIsPlaying() && (this->selectedAlgorithm == Algorithm::GAMETREE))
			{
				this->UpdateRootNode(f);
				this->CleanUpTree();
			}

			this->SwitchPlayer();
			return true;
		}
		return false;
	}

	// Returns True if game is over.
	bool Game::IsGameOver() const
	{
		return (this->board.Winner() != Player::NONE) || this->board.IsBoardFull();
	}

	// The score of game state.
	int Game::MinMaxScore(const GameBoard& board, const int& depth) const
	{
		if (board.CheckIfPlayerWon(this->CurrentPlayer()))
		{
			return 10 - depth;
		}
		else if (board.CheckIfPlayerWon(this->CurrentPlayer() == Player::CROSS ? Player::CIRCLE : Player::CROSS))
		{
			return depth - 10;
		}
		else
		{
			return 0;
		}
	}

	// Returns the value of possible game state.
	int Game::MinMax(GameBoard board, Player player, int depth) const
	{
		if ((board.Winner() != Player::NONE) || board.IsBoardFull())
		{
			return this->MinMaxScore(board, depth);
		}
		else
		{
			depth++;
			player = player == Player::CROSS ? Player::CIRCLE : Player::CROSS;
			std::map<short, int> availableMovements;
			for (size_t i = 0; i < board.fields.size(); i++)
			{
				if (board.IsFieldFree(i))
				{
					GameBoard newBoard = board;
					newBoard.SetField(i, player);
					availableMovements.emplace(std::make_pair(i, this->MinMax(newBoard, player, depth)));
				}
			}

			if (this->CurrentPlayer() == player)
			{
				auto maxMove = std::max_element(availableMovements.begin(), availableMovements.end(),
					[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
					return p1.second < p2.second; })->first;
				return availableMovements[maxMove];
			}
			else
			{
				auto minMove = std::min_element(availableMovements.begin(), availableMovements.end(),
					[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
					return p1.second < p2.second; })->first;
				return availableMovements[minMove];
			}
		}
	}

	// Returns the number of the best available move for the current player.
	int Game::MinMaxBestMove() const
	{
		std::map<short, int> availableMovements;
		for (size_t i = 0; i < this->board.fields.size(); i++)
		{
			if (this->board.IsFieldFree(i))
			{
				GameBoard newBoard = this->board;
				newBoard.SetField(i, this->CurrentPlayer());
				availableMovements.emplace(std::make_pair(i, this->MinMax(newBoard, this->CurrentPlayer(), 1)));
			}
		}

		return std::max_element(availableMovements.begin(), availableMovements.end(),
			[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
			return p1.second < p2.second; })->first;
	}

	// Returns True if move is allowed
	bool Game::MoveIsAllowed(const int& move) const
	{
		return this->board.IsFieldFree(move);
	}
}
