#pragma once
#include "Game.h"
// Timer identifier
#define GAME_TIMER 1

// Ctor
Game::Game() {
	// Load game stat records
	stat.LoadStat();
	model = NULL;
	nTimerID = -1;
	gameStart = false;
	gameOver = false;
}
// Dtor
Game::~Game() { delete model; }

void Game::InitGame(int FieldWidth, int FieldHeight) {
	gameStart = gameOver = false;
	// Create game model
	model = new Model(FieldWidth, FieldHeight);
	// Assign model to view
	view->ReSetModel(model);
}

// Start game
void Game::GameStart()
{
	if(model == NULL) return;
	gameStart = true;
	gameOver = false;
	TimerStart();	
}

// Stop game
void Game::EndGame() {
	gameStart = false;
	gameOver = true;
	TimerStop();
	view->ReleaseModel();
	model = NULL;	
}

bool Game::IsStart() { return gameStart; }
bool Game::IsGameOver() { return gameOver; }

GameState Game::GameProcess(int x, int y) {
	// Redraw items
	view->DrawGLScene();
	SwapBuffers(view->hDC);

	// We open cell with bomb
	if (model->GetCell(x, y).state == CellState::OPEN && model->GetCell(x, y).hasMine) {
		// Create game stat record
		stat.AddRecord(StatRec(model->Width, model->Height, model->BombCount, model->timer, false));
		// Return game status
		return GameState::CompWin;
	}
	// Game successfully end
	else if(model->FlagCount == model->BombCount && model->FlagsCorrect()) {
		stat.AddRecord(StatRec(model->Width, model->Height, model->BombCount, model->timer, true));
		return GameState::PlayerWin;
	}
	// Otherwise game continue
	return GameState::Continue;
}

void Game::TimerStart() { nTimerID = SetTimer(view->hWnd, GAME_TIMER, 1000, NULL); }
void Game::TimerStop() { KillTimer(view->hWnd, GAME_TIMER); }
void Game::UpdateTimer() { model->timer++; }

Model * Game::GetModel() { return model; }
View * Game::GetView() { return view; }
Statistic & Game::GetStat() { return stat; }

void Game::SetView(View *v) { view = v; }
