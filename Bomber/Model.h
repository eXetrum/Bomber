#pragma once
#include <vector>
#include <time.h>
using namespace std;

enum CellState {CLOSED = 0, OPEN, FLAG};
enum GameState {Continue, PlayerWin, CompWin};
// Predefinition field size
#define FIELD_TYPE0 10
#define FIELD_TYPE1 15
// Field cell
struct Cell {
	// Cell status: {closed, opened, flaged, mined}
	CellState state;
	bool hasMine;
};
// Game model
class Model {
public:
	Model();
	// Ctor
	Model(int, int);
	// Copy ctor
	Model(const Model &);
	// Dtor
	~Model();
	// Bombs setup
	void SetMines();
	// Chance cell state
	void SetCell(int, int, CellState);
	// Return field cell at {x, y}
	Cell &GetCell(int x, int y);
	// Open cell at {x, y}
	void Open(int x, int y);
	// Mark cell at {x, y}
    void Mark(int x, int y);
	// Check if each flag at apropriate place
	bool FlagsCorrect();
	// Field params
	int Width, Height;	
	int BombCount;
	int FlagCount;
	// Game time
	long timer;
	// Empty domain structure
	class Domain {
	public:
		// Ctor
		Domain() {}
		// Start domen point
		Domain(pair<int, int> startPoint, size_t m, size_t n)
		{
			points.push_back(startPoint);
			this->m = (int)m;
			this->n = (int)n;
		}
		// Dtor
		~Domain() { }
		// Append all domen points
		int ** add_neighbors(int ** p, int m, int n) {
			// Get start point
			pair<int, int> k = points.at(0);
			this->m = m;
			this->n = n;
			p[k.first][k.second] = -1;
			// Check right cell
			if(k.second + 1 < n)
			{
				if(p[k.first][k.second + 1] == 1)
					add_neighbor(p, pair<int, int>(k.first, k.second + 1));
			}
			// Check bottom cell
			if(k.first + 1 < m)
			{
				if(p[k.first + 1][k.second] == 1)
					add_neighbor(p, pair<int, int>(k.first + 1, k.second));
			}
			// Check left cell
			if(k.second - 1 >= 0)
			{
				if(p[k.first][k.second - 1] == 1)
					add_neighbor(p, pair<int, int>(k.first, k.second - 1));
			}
			// Check up cell
			if(k.first - 1 >= 0)
			{
				if(p[k.first - 1][k.second] == 1)
					add_neighbor(p, pair<int, int>(k.first - 1, k.second));
			}
			return p;
		}

		void add_neighbor(int **p, pair<int, int> k) {
			bool already = false;
			// Prevent duplicate points
			for(size_t i = 0; i < points.size(); i++)
				// Found duplicate
				if(points.at(i).first == k.first
					&& points.at(i).second == k.second)
					return;
			// Otherwise
			// Append new point to the domain
			points.push_back(k);
			// Remove from matrix
			p[k.first][k.second] = -1;			
			// Check right	
			if(k.second + 1 < n)
			{
				if(p[k.first][k.second + 1] == 1)
					add_neighbor(p, pair<int, int>(k.first, k.second + 1));
			}
			// Check bottom
			if(k.first + 1 < m)
			{
				if(p[k.first + 1][k.second] == 1)
					add_neighbor(p, pair<int, int>(k.first + 1, k.second));
			}
			// Check left
			if(k.second - 1 >= 0)
			{
				if(p[k.first][k.second - 1] == 1)
					add_neighbor(p, pair<int, int>(k.first, k.second - 1));
			}
			// Check up
			if(k.first - 1 >= 0)
			{
				if(p[k.first - 1][k.second] == 1)
					add_neighbor(p, pair<int, int>(k.first - 1, k.second));
			}
		}
		// Domain points
		vector<pair<int, int>> points;
	private:
		int m, n;
	};
private:	
	// Game field
	Cell * field;
	int **domainMatrix;
	vector<Domain> domains;
};