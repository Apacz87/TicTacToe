#include "stdafx.h"
#include "TicTacGame.h"
#include <thread>

namespace TicTacGame
{
	// Number of existing game tree nodes.
	int GameNode::m_numberOfNodes = 0;

	// The GameBoard class constructor.
	GameBoard::GameBoard()
	{
		this->m_fields.fill(Player::NONE);
	}

	// Checking if game is over, returns the winner.
	Player GameBoard::winner() const
	{
		// Auxiliary variables
		bool firstLine, secondLine, thirdLine, firstColumn, secondColumn, thirdColumn, firstDiagonal, secondDiagonal;

		// Checking for success in the line:
		firstLine = ((this->m_fields[0] == this->m_fields[1]) && (this->m_fields[1] == this->m_fields[2]) && (this->m_fields[0] != Player::NONE));
		secondLine = ((this->m_fields[3] == this->m_fields[4]) && (this->m_fields[4] == this->m_fields[5]) && (this->m_fields[3] != Player::NONE));
		thirdLine = ((this->m_fields[6] == this->m_fields[7]) && (this->m_fields[7] == this->m_fields[8]) && (this->m_fields[6] != Player::NONE));

		// Checking success in the column:
		firstColumn = ((this->m_fields[0] == this->m_fields[3]) && (this->m_fields[3] == this->m_fields[6]) && (this->m_fields[0] != Player::NONE));
		secondColumn = ((this->m_fields[1] == this->m_fields[4]) && (this->m_fields[4] == this->m_fields[7]) && (this->m_fields[1] != Player::NONE));
		thirdColumn = ((this->m_fields[2] == this->m_fields[5]) && (this->m_fields[5] == this->m_fields[8]) && (this->m_fields[2] != Player::NONE));

		// Checking success diagonally:
		firstDiagonal = ((this->m_fields[0] == this->m_fields[4]) && (this->m_fields[4] == this->m_fields[8]) && (this->m_fields[0] != Player::NONE));
		secondDiagonal = ((this->m_fields[2] == this->m_fields[4]) && (this->m_fields[4] == this->m_fields[6]) && (this->m_fields[2] != Player::NONE));

		if (!(firstLine || secondLine || thirdLine || firstColumn || secondColumn || thirdColumn || firstDiagonal || secondDiagonal))
			return Player::NONE; // No winner

		// Checking who won:
		if (firstLine) return this->m_fields[1];
		if (secondLine) return this->m_fields[3];
		if (thirdLine) return this->m_fields[6];
		if (firstColumn) return this->m_fields[3];
		if (secondColumn) return this->m_fields[4];
		if (thirdColumn) return this->m_fields[5];
		if (firstDiagonal) return this->m_fields[4];
		// secondDiagonal varible must be TRUE.
		return this->m_fields[4];
	}

	// Check that the indicated field is free.
	bool GameBoard::isFieldFree(const int& t_fieldNumber) const
	{
		if (t_fieldNumber<0 || t_fieldNumber>8) return false;
		return this->m_fields[t_fieldNumber] == Player::NONE;
	}

	// Check whether the indicated player won.
	inline bool GameBoard::checkIfPlayerWon(const Player& t_player) const
	{
		return this->winner() == t_player;
	}

	// Check if game board is full.
	inline bool GameBoard::isBoardFull() const
	{
		return this->m_fields.end() == std::find(this->m_fields.begin(), this->m_fields.end(), Player::NONE);
	}

	// Return number of occupied fields in board game.
	inline int GameBoard::numberOfOccupiedFields() const
	{
		return std::count_if(this->m_fields.begin(), this->m_fields.end(), [&](Player ply){ return ply != Player::NONE; });
	}

	// GameBoard subscript operator.
	Player& GameBoard::operator[](std::size_t t_index)
	{
		if (t_index < 0 || t_index>8)
		{
			throw std::out_of_range("Field value is out of reange!");
		}

		return this->m_fields[t_index];
	}

	// GameBoard subscript operator.
	const Player& GameBoard::operator[](std::size_t t_index) const
	{
		if (t_index < 0 || t_index>8)
		{
			throw std::out_of_range("Field value is out of reange!");
		}

		return this->m_fields[t_index];
	}

	// Returns size of game board.
	inline std::size_t GameBoard::size() const
	{
		return this->m_fields.size();
	}

	// The GameNode class constructor.
	GameNode::GameNode(Player t_player = Player::CROSS) : m_gameState(), m_lastMove(-1), m_currentPlayer(t_player)
	{
		this->m_numberOfNodes++;
	}

	// The GameNode class constructor.
	GameNode::GameNode(std::shared_ptr<GameNode> t_base, Player t_player, GameBoard t_board, short t_move) :
		m_gameState(t_board), m_lastMove(t_move), m_parentNode(t_base), m_currentPlayer(t_player)
	{
		this->m_numberOfNodes++;
	}

	// The GameNode class destructor.
	GameNode::~GameNode()
	{
		//std::lock_guard<std::mutex> lg(this->nodeMutex);
		this->deleteChildNodes();
		this->m_numberOfNodes--;
	}

	// Add node to derived nodes list of a current node.
	void GameNode::addChildNode(GameBoard t_board, const int& t_field)
	{
		t_board[t_field] = this->m_currentPlayer;
		auto nextPlayer = this->m_currentPlayer == Player::CROSS ? Player::CIRCLE : Player::CROSS;
		this->m_derivedNodes.emplace_back(std::make_shared<GameNode>(shared_from_this(), nextPlayer, t_board, t_field));
	}

	// Generate child nodes for current node.
	void GameNode::generateChildNodes()
	{
		if (!this->leaf())
		{
			std::lock_guard<std::mutex> lg(this->m_nodeMutex);
			this->m_derivedNodes.reserve(this->m_gameState.size() - this->m_gameState.numberOfOccupiedFields());
			for (int i = 0; i < 9; i++)
			{
				if (this->m_gameState.isFieldFree(i))
				{
					this->addChildNode(this->m_gameState, i);
				}
			}
		}
	}

	// Delete all derived nodes of a current node.
	void GameNode::deleteChildNodes()
	{
		std::lock_guard<std::mutex> lg(this->m_nodeMutex);
		this->m_derivedNodes.clear();
	}

	// Return number of existing nodes.
	inline int GameNode::totalNumberOfNodes() const
	{
		return this->m_numberOfNodes;
	}

	// Returns True if the node is a leaf.
	inline bool GameNode::leaf() const
	{
		return (this->m_gameState.winner() != Player::NONE) || this->m_gameState.isBoardFull();
	}

	// Recursive generating child nodes in game tree.
	void GameNode::recursiveTreeWalk(std::shared_ptr<GameNode> t_partialTree)
	{
		if (!t_partialTree->leaf())
		{
			if (t_partialTree->m_derivedNodes.empty())
			{
				t_partialTree->generateChildNodes();
			}

			for each (auto node in t_partialTree->m_derivedNodes)
			{
				node->recursiveTreeWalk(node);
			}
		}
	}

	// Return value of game tree node.
	int GameNode::nodeVale() const
	{
		if (this->leaf())
		{
			if (this->m_gameState.checkIfPlayerWon(Player::CROSS))
			{
				return 10 - this->distanceFromRoot();
			}
			else if (this->m_gameState.checkIfPlayerWon(Player::CIRCLE))
			{
				return this->distanceFromRoot() - 10;
			}

			return 0;
		}
		else if (this->m_derivedNodes.empty())
		{
			return 0;
		}

		if (this->m_currentPlayer == Player::CROSS)
		{
			return (*std::max_element(this->m_derivedNodes.begin(), this->m_derivedNodes.end(),
				[](const std::shared_ptr<GameNode> p1, const std::shared_ptr<GameNode> p2) {
				return p1->nodeVale() < p2->nodeVale(); }))->nodeVale();
		}

		return (*std::min_element(this->m_derivedNodes.begin(), this->m_derivedNodes.end(),
			[](const std::shared_ptr<GameNode> p1, const std::shared_ptr<GameNode> p2) {
			return p1->nodeVale() < p2->nodeVale(); }))->nodeVale();
	}

	// Returns value of base move.
	inline short GameNode::baseMove() const
	{
		return this->m_lastMove;
	}

	// Returns distance from root node.
	inline int GameNode::distanceFromRoot() const
	{
		if (!this->m_parentNode.expired())
		{
			return 1 + this->m_parentNode.lock()->distanceFromRoot();
		}

		return 0;
	}

	// The Game class constructor.
	Game::Game(const GameSettings& settings) : m_availableMovements(), m_playWithAI(settings.AI), m_selectedAlgorithm(settings.Implementation), m_player(Player::CROSS)
	{
		if (this->m_playWithAI && (this->m_selectedAlgorithm == Algorithm::GAMETREE))
		{
			this->m_threadGeneratingGameTree = std::async(&Game::generateTree, this);
		}
	}

	// The Game class destructor.
	Game::~Game()
	{
		if (this->m_rootNode != nullptr)
		{
			this->m_rootNode->deleteChildNodes();
		}
	}

	// Returns True if AI is Playing.
	inline bool Game::isAiPlaying() const
	{
		return this->m_playWithAI;
	}

	// Returns True if specified player won.
	bool Game::checkIfSpecifiedPlayerWon(const Player& ply) const
	{
		return this->m_board.checkIfPlayerWon(ply);
	}

	// Returns number of existing nodes in game tree.
	int Game::numberOfExistingNodes() const
	{
		return this->m_rootNode->totalNumberOfNodes();
	}

	// Switch current player.
	inline void Game::switchPlayer(){ this->m_player = (this->m_player == Player::CROSS ? Player::CIRCLE : Player::CROSS); }

	// Returns current Player.
	inline Player Game::currentPlayer() const
	{
		return this->m_player;
	}

	// Generate game tree.
	std::shared_ptr<GameNode> Game::generateTree()
	{
		auto newGameTreeRoot = std::make_shared<GameNode>(Player::CROSS);
		newGameTreeRoot->recursiveTreeWalk(newGameTreeRoot);
		return newGameTreeRoot;
	}

	// Update available movements list.
	void Game::updateAvailableMovements()
	{
		this->m_availableMovements.clear();
		for each (auto node in this->m_rootNode->m_derivedNodes)
		{
			this->m_availableMovements.emplace(std::make_pair(node->baseMove(), node->nodeVale()));
		}
	}

	// Update the root node in tree.
	void Game::updateRootNode(const short& move)
	{
		if (this->m_rootNode == nullptr)
		{
			this->m_rootNode = this->m_threadGeneratingGameTree.get();
		}

		auto selectedNode = std::find_if(this->m_rootNode->m_derivedNodes.begin(), this->m_rootNode->m_derivedNodes.end(), [move](std::shared_ptr<GameNode> node){ return node->baseMove() == move; });
		this->m_rootNode = selectedNode->get()->shared_from_this();
		this->updateAvailableMovements();
	}

	// Delete the old Nodes.
	inline void Game::deleteOldNodes(std::shared_ptr<GameNode> oldRoot)
	{
		oldRoot->deleteChildNodes();
	}

	// Delete unreachable game tree nodes.
	void Game::cleanUpTree()
	{
		if (!this->m_rootNode->m_parentNode.expired())
		{
			auto oldRootNode = this->m_rootNode->m_parentNode.lock();
			std::thread cleanUpThread(&Game::deleteOldNodes, this, oldRootNode);
			cleanUpThread.detach();
		}
	}

	// Returns the number of the best available move for the indicated player.
	int Game::bestAvailableMove(const Player& player) const
	{
		if (this->m_selectedAlgorithm == Algorithm::GAMETREE)
		{
			if (player == Player::CROSS)
			{
				return std::max_element(this->m_availableMovements.begin(), this->m_availableMovements.end(),
					[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
					return p1.second < p2.second; })->first;
			}

			return std::min_element(this->m_availableMovements.begin(), this->m_availableMovements.end(),
				[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
				return p1.second < p2.second; })->first;
		}
		else
		{
			return this->minMaxBestMove();
		}
	}

	// Returns the number of the best available move for the current player.
	int Game::bestAvailableMove()
	{
		return this->bestAvailableMove(this->currentPlayer());
	}

	// Make move in game board and returns true if succeed.
	void Game::makeMove(const int& f)
	{
		if (!this->m_board.isFieldFree(f))
		{
			throw std::invalid_argument("The selected field is already occupied!");
		}
		
		this->m_board[f] = this->currentPlayer();
		if (this->isAiPlaying() && (this->m_selectedAlgorithm == Algorithm::GAMETREE))
		{
			this->updateRootNode(f);
			this->cleanUpTree();
		}

		this->switchPlayer();
	}

	// Returns True if game is over.
	bool Game::isGameOver() const
	{
		return (this->m_board.winner() != Player::NONE) || this->m_board.isBoardFull();
	}

	// The score of game state.
	inline int Game::minMaxScore(const GameBoard& board, const int& depth) const
	{
		if (board.checkIfPlayerWon(this->currentPlayer()))
		{
			return 10 - depth;
		}
		else if (board.checkIfPlayerWon(this->currentPlayer() == Player::CROSS ? Player::CIRCLE : Player::CROSS))
		{
			return depth - 10;
		}
		else
		{
			return 0;
		}
	}

	// Returns the value of possible game state.
	int Game::minMax(GameBoard board, Player player, int depth) const
	{
		if ((board.winner() != Player::NONE) || board.isBoardFull())
		{
			return this->minMaxScore(board, depth);
		}
		else
		{
			depth++;
			player = player == Player::CROSS ? Player::CIRCLE : Player::CROSS;
			std::map<short, int> availableMovements;
			for (size_t i = 0; i < board.size(); i++)
			{
				if (board.isFieldFree(i))
				{
					GameBoard newBoard = board;
					newBoard[i] = player;
					availableMovements.emplace(std::make_pair(i, this->minMax(newBoard, player, depth)));
				}
			}

			if (this->currentPlayer() == player)
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
	int Game::minMaxBestMove() const
	{
		std::map<short, int> availableMovements;
		for (size_t i = 0; i < this->m_board.size(); i++)
		{
			if (this->m_board.isFieldFree(i))
			{
				GameBoard newBoard = this->m_board;
				newBoard[i] = this->currentPlayer();
				availableMovements.emplace(std::make_pair(i, this->minMax(newBoard, this->currentPlayer(), 1)));
			}
		}

		return std::max_element(availableMovements.begin(), availableMovements.end(),
			[](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
			return p1.second < p2.second; })->first;
	}

	// Returns True if move is allowed
	bool Game::isMoveAllowed(const int& move) const
	{
		return this->m_board.isFieldFree(move);
	}
}
