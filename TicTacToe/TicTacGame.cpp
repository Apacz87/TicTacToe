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

	// Checking if game is over, someone won.
	int GameBoard::Win()
	{
		bool a, b, c, d, e, f, g, h;

		// sukces w linii?

		a = ((fields[0] == fields[1]) && (fields[1] == fields[2]) && (fields[0] != Player::NONE));
		b = ((fields[3] == fields[4]) && (fields[4] == fields[5]) && (fields[3] != Player::NONE));
		c = ((fields[6] == fields[7]) && (fields[7] == fields[8]) && (fields[6] != Player::NONE));
		// sukces w kolumnie?

		d = ((fields[0] == fields[3]) && (fields[3] == fields[6]) && (fields[0] != Player::NONE));
		e = ((fields[1] == fields[4]) && (fields[4] == fields[7]) && (fields[1] != Player::NONE));
		f = ((fields[2] == fields[5]) && (fields[5] == fields[8]) && (fields[2] != Player::NONE));

		// sukces na przekatnej

		g = ((fields[0] == fields[4]) && (fields[4] == fields[8]) && (fields[0] != Player::NONE));
		h = ((fields[2] == fields[4]) && (fields[4] == fields[6]) && (fields[2] != Player::NONE));

		if (!(a || b || c || d || e || f || g || h))
			return 0;

		if (a) return 1;    // robimy to aby wiedziec kto wygral
		if (b) return 3;
		if (c) return 6;
		if (d) return 3;
		if (e) return 4;
		if (f) return 5;
		if (g) return 4;
		if (h) return 4;
	}

	// Check that the indicated field is free.
	bool GameBoard::IsFieldFree(int nr)
	{
		if (nr<0 || nr>8) return false;
		return fields[nr] == Player::NONE;
	}

	// Sets the value on the indicated field.
	void GameBoard::SetField(int nr, char player)
	{
		if (nr<0 || nr>8) std::out_of_range("Field value is out of reange!");
		if (player == 'X') fields[nr] = Player::CROSS;
		else fields[nr] = Player::CIRCLE;
	}

	// Check whether the indicated player won.
	bool GameBoard::CheckIfPlayerWon(Player player)
	{
		auto fieldNumber = this->Win();
		if (fieldNumber < 1)
		{
			return false;
		}

		return this->fields[fieldNumber] == player;
	}

	// Check if game board is full.
	bool GameBoard::IsBoardFull()
	{
		return this->fields.end() == std::find(this->fields.begin(), this->fields.end(), Player::NONE);
	}

	// Return number of occupied fields in board game.
	int GameBoard::NumberOfOccupiedFields()
	{
		return std::count_if(this->fields.begin(), this->fields.end(), [](Player ply){ return ply != Player::NONE; });
	}

	// The GameNode class constructor.
	GameNode::GameNode(Player ply = Player::CROSS) : gameState(), lastMove(-1)
	{
		this->currentPlayer = ply;
		this->numberOfNodes++;
	}

	// The GameNode class constructor.
	GameNode::GameNode(std::shared_ptr<GameNode> base, Player ply, GameBoard board, short move)
	{
		this->lastMove = move;
		this->parentNode = base;
		this->gameState = board;
		this->currentPlayer = ply;
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
	void GameNode::AddChildNode(GameBoard board, int field)
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
	int GameNode::TotalNumberOfNodes()
	{
		return this->numberOfNodes;
	}

	// Returns True if the node is a leaf.
	bool GameNode::Leaf()
	{
		return this->gameState.Win();
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
	int GameNode::NodeVale()
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
	short GameNode::BaseMove()
	{
		return this->lastMove;
	}

	// Returns distance from root node.
	int GameNode::DistanceFromRoot()
	{
		if (!this->parentNode.expired())
		{
			return 1 + this->parentNode.lock()->DistanceFromRoot();
		}
		return 0;
	}

	// The Game class constructor.
	Game::Game(bool ai = false) : availableMovements()
	{
		this->playWithAI = ai;
		this->player = Player::CROSS;
		this->gameTree = nullptr;
		this->rootNode = gameTree;
		if (this->playWithAI)
		{
			this->threadGeneratingGameTree = std::async(&Game::generateTree, this);
		}
	}

	// Returns True if AI is Playing.
	bool Game::AiIsPlaying()
	{
		return this->playWithAI;
	}

	// Returns True if specified player won.
	bool Game::CheckIfSpecifiedPlayerWon(Player ply)
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
	Player Game::CurrentPlayer()
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
	void Game::UpdateRootNode(short move)
	{
		if (this->rootNode == nullptr)
		{
			this->gameTree = this->threadGeneratingGameTree.get();
			this->rootNode = this->gameTree;
		}

		auto selectedNode = std::find_if(this->rootNode->derivedNodes.begin(), this->rootNode->derivedNodes.end(), [move](std::shared_ptr<GameNode> node){ return node->BaseMove() == move; });
		this->rootNode = selectedNode->get()->shared_from_this();
		this->UpdateAvailableMovements();
		this->SwitchPlayer();
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

	// Returns the number of the best available move for the current player.
	int Game::BestAvailableMove()
	{
		return this->BestAvailableMove(this->CurrentPlayer());
	}

	// Make move in game board and returns true if succeed.
	bool Game::MakeMove(int f)
	{
		if (this->board.IsFieldFree(f))
		{
			this->board.SetField(f, this->CurrentPlayer());
			return true;
		}
		return false;
	}

	// Returns True if game is over.
	bool Game::IsGameOver()
	{
		if (this->board.Win() || this->board.IsBoardFull()) return true;
		return false;
	}
}