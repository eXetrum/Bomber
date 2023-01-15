#pragma once
#include "Model.h"
#include "View.h"
#include "Statistic.h"
// Game class
class Game {
public:
	// Ctor
	Game();
	// Dtor
	~Game();
	// Game initialization
	void InitGame(int, int);
	// Start game
	void GameStart();
	// Stop game
	void EndGame();
	// Check if game already start or already over
	bool IsStart();
	bool IsGameOver();
	// Proceed game next move
	GameState GameProcess(int x, int y);	
	// Start/stop/update timer
	void TimerStart();
	void TimerStop();
	void UpdateTimer();
	// Setup view object
	void SetView(View *);
	// Accessors
	Model * GetModel();
	View * GetView();
	Statistic &GetStat();
private:
	bool gameStart;
	bool gameOver;

	Model *model;
	View *view;
	Statistic stat;
	int nTimerID;	
};