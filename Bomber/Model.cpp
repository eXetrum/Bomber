#pragma once
#include "Model.h"
#include <fstream>

Model::Model() {}

Model::Model(int width = 10, int height = 10) {
	timer = 0;
	Width = width;
	Height = height;
	// Random number of bombs
	BombCount = (int)sqrt(width * height);
	BombCount += rand() % BombCount;
	// No flags yet
	FlagCount = 0;
	// Create field cells
	field = new Cell[width * height];
	// Place bombs
	SetMines();
	// Create domain patrix
	domainMatrix = new int*[height];
	for(int i = 0; i < height; i++)
		domainMatrix[i] = new int[width];

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			int minesAround = 0;
            for (int yy = y - 1; yy <= y + 1; ++yy)
			{
                for (int xx = x - 1; xx <= x + 1; ++xx)
                {
                    if ((xx == x && yy == y) ||
						xx < 0 || xx >= width ||
						yy < 0 || yy >= height)
                        continue;
					if (GetCell(xx, yy).hasMine)
                        ++minesAround;
                }
			}
			if(GetCell(x, y).state == CellState::CLOSED &&
				GetCell(x, y).hasMine == false &&
				minesAround == 0)
			{
				domainMatrix[y][x] = 1;
			}
			else
			{
				domainMatrix[y][x] = 0;
			}
		}
	}
	
	
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			if(domainMatrix[y][x] == 1)
			{
				// Right side neighbours
				if(x + 1 < width)
				{
					// Chick right
					if(domainMatrix[y][x + 1] != 1 && domainMatrix[y][x + 1] == 0)
						domainMatrix[y][x + 1] = 2;
					// Chick up
					if(y + 1 < height)
						if(domainMatrix[y + 1][x + 1] != 1 && domainMatrix[y + 1][x + 1] == 0)
							domainMatrix[y + 1][x + 1] = 2;
					// Chick bottom
					if(y - 1 >= 0)
						if(domainMatrix[y - 1][x + 1] != 1 && domainMatrix[y - 1][x + 1] == 0)
							domainMatrix[y - 1][x + 1] = 2;
				}
				// Left side neighbours
				if(x - 1 >= 0)
				{
					if(domainMatrix[y][x - 1] != 1 && domainMatrix[y][x - 1] == 0)
						domainMatrix[y][x - 1] = 2;
					if(y + 1 < height)
						if(domainMatrix[y + 1][x - 1] != 1 && domainMatrix[y + 1][x - 1] == 0)
							domainMatrix[y + 1][x - 1] = 2;
					if(y - 1 >= 0)
						if(domainMatrix[y - 1][x - 1] != 1 && domainMatrix[y - 1][x - 1] == 0)
							domainMatrix[y - 1][x - 1] = 2;
				}
				if(y + 1 < height)
					if(domainMatrix[y + 1][x] != 1 && domainMatrix[y + 1][x] == 0)
						domainMatrix[y + 1][x] = 2;
				if(y - 1 >= 0)
					if(domainMatrix[y - 1][x] != 1 && domainMatrix[y - 1][x] == 0)
						domainMatrix[y - 1][x] = 2;
			}
		}
	}
	// Get all domains
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++)
		{
			// Found start point
			if(domainMatrix[i][j] == 1)
			{
				// Create new domain
				Domain d(pair<int, int>(i, j), height, width);
				// Get all domain points
				domainMatrix = d.add_neighbors(domainMatrix, height, width);
				// Append domain into domains
				domains.push_back(d);
			}
		}
	}
}

// Copy ctor
Model::Model(const Model &href) {
	timer = href.timer;
	field = new Cell[href.Width * href.Height];

	Width = href.Width;
	Height = href.Height;
	BombCount = href.BombCount;
	FlagCount = href.FlagCount;
	// Copy cells info
	for (int x = 0; x < Width; x++)
		for (int y = 0; y < Height; y++)
		{
			field[y * Width + x].hasMine = href.field[y * Width + x].hasMine;
			field[y * Width + x].state = href.field[y * Width + x].state;
		}
	// Copy domain info
	domains.assign(href.domains.begin(), href.domains.end());
}

// Dtor
Model::~Model() {
	delete []field;
	
	for(int i = 0; i < Height; i++)
		delete []domainMatrix[i];
	delete []domainMatrix;
}

void Model::SetMines() {
	srand((unsigned)time(NULL));

	for (int y = 0; y < Height; y++)
		for (int x = 0; x < Width; x++)
        {
			// Initialy cell is closed
			field[y * Width + x].state = CellState::CLOSED;
			// No bombs yet
			field[y * Width + x].hasMine = false;
        }
	// Place bombs
	int count = 0;
	while(count < BombCount)
    {
		// Generate new bomb coords
        int x, y;
        do
        {
			x = rand() % Width;
			y = rand() % Height;
        }
		// Repeat untill we reach empty cell
		while (GetCell(x, y).hasMine);
		GetCell(x, y).hasMine = true;
		count++;
    }
}

// Change cell state
void Model::SetCell(int x, int y, CellState state) { field[y * Width + x].state = state; }

// Return field cell by coords
Cell &Model::GetCell(int x, int y) { return field[y * Width + x]; }

// Open field cell
void Model::Open(int x, int y) {
	// We can open only closed cells
	if(GetCell(x, y).state != CellState::CLOSED) return;
	int domainID = -1;
	// Check if current {x, y} point lie in some domain
	for(size_t i = 0; i < domains.size() && domainID == -1; i++) {
		// Check domain points
		for(size_t j = 0; j < domains.at(i).points.size() && domainID == -1; j++)
			// Found point in current domain
			if(domains.at(i).points.at(j).first == y && domains.at(i).points.at(j).second == x)
				// Remember domain index
				domainID = i;
	}
	// If point lie in some domain
	if(domainID != -1)
	{
		// Open all cells that this domain contain
		for(size_t j = 0; j < domains.at(domainID).points.size(); j++)
		{
			// Open current cell
			GetCell(domains.at(domainID).points.at(j).second, domains.at(domainID).points.at(j).first).state = CellState::OPEN;
			// Open right side
			if(domains.at(domainID).points.at(j).second + 1 < Width)
			{
				// right
				GetCell(domains.at(domainID).points.at(j).second + 1, domains.at(domainID).points.at(j).first).state = CellState::OPEN;
				// Up
				if(domains.at(domainID).points.at(j).first + 1 < Height)
					GetCell(domains.at(domainID).points.at(j).second + 1, domains.at(domainID).points.at(j).first + 1).state = CellState::OPEN;
				// Bottom
				if(domains.at(domainID).points.at(j).first - 1 >= 0)
					GetCell(domains.at(domainID).points.at(j).second + 1, domains.at(domainID).points.at(j).first - 1).state = CellState::OPEN;
			}
			// Open left side
			if(domains.at(domainID).points.at(j).second - 1 >= 0)
			{
				// Left
				GetCell(domains.at(domainID).points.at(j).second - 1, domains.at(domainID).points.at(j).first).state = CellState::OPEN;
				// Up
				if(domains.at(domainID).points.at(j).first + 1 < Height)
					GetCell(domains.at(domainID).points.at(j).second - 1, domains.at(domainID).points.at(j).first + 1).state = CellState::OPEN;
				// Bottom
				if(domains.at(domainID).points.at(j).first - 1 >= 0)
					GetCell(domains.at(domainID).points.at(j).second - 1, domains.at(domainID).points.at(j).first - 1).state = CellState::OPEN;
			}
			// Open top side
			if(domains.at(domainID).points.at(j).first - 1 >= 0)
			{
				GetCell(domains.at(domainID).points.at(j).second, domains.at(domainID).points.at(j).first - 1).state = CellState::OPEN;
			}
			// Open bottom side
			if(domains.at(domainID).points.at(j).first + 1 < Height)
				GetCell(domains.at(domainID).points.at(j).second, domains.at(domainID).points.at(j).first + 1).state = CellState::OPEN;
		}
	}
	// Open current cell
	GetCell(x, y).state = CellState::OPEN;
}
// Mark cell with flag
void Model::Mark(int x, int y) {
	switch (GetCell(x, y).state)
    {
	case CellState::OPEN:
        break;
    case CellState::CLOSED:
		if (FlagCount == BombCount) return;
		FlagCount++;
		GetCell(x, y).state = CellState::FLAG;
        break;
    case CellState::FLAG:
		if(FlagCount > 0)
			FlagCount--;
		GetCell(x, y).state = CellState::CLOSED;
        break;
    }
}

// Check flags places
bool Model::FlagsCorrect() {
	int correctFlags = 0;
	for(int y = 0; y < Height; y++)
		for(int x = 0; x < Width; x++)
			// Flag correct if current cell contain bomb
			if(GetCell(x, y).hasMine && GetCell(x, y).state == CellState::FLAG)
				correctFlags++;
	return correctFlags == BombCount;
}