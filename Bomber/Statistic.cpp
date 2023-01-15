#pragma once
#include "Statistic.h"
#include <windows.h>

// Default ctor
StatRec::StatRec() {}

// Parametrized ctor
StatRec::StatRec(int Width, int Height, int BombCount, long gameTime, bool WinGame) {
	// Get elapsed time
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	// Get current date
	strftime(recDate, 80 * sizeof(char), "%A, %B %d, %Y %H:%M:%S", timeinfo);
	// Copy field settings
	this->Width = Width;
	this->Height = Height;
	this->BombCount = BombCount;
	// Copy gametime and game result
	this->gameTime = gameTime;
	this->WinGame = WinGame;
}

// Copy ctor
StatRec::StatRec(const StatRec &href) {
	strcpy(recDate, href.recDate);
	// Copy game field params
	Width = href.Width;
	Height = href.Height;
	BombCount = href.BombCount;
	// Copy game time (elapsed time) and game result(win/loose)
	gameTime = href.gameTime;
	WinGame = href.WinGame;
}
// Covert record to string respresentation
string StatRec::ToString() {
	char buff[32] = { 0 };
	string r = "";
	r += recDate; r += "\t";
	_itoa(Width, buff, 10);
	r += buff; r += "\t";
	_itoa(Height, buff, 10);
	r += buff; r += "\t";
	_itoa(BombCount, buff, 10);
	r += buff; r += "\t";
	_itoa(gameTime, buff, 10);
	r += buff; r += "\t";
	r += (WinGame) ? "Win" : "Loose";
	return r;
}

// Ctor
Statistic::Statistic()  {
	// Total games with "win" result
	wins = 0;
	// Total games with "loose" result
	looses = 0;
	// Total elapsed time
	TotalTime = 0;
	// Longest game
	slowGame = 0;
	// Shortest game
	quickGame = MAXLONG;
	// Filepath
	savePath = "Stat.dat";
}
// Dtor
Statistic::~Statistic() { Clear(); }

// Add new stat record
void Statistic::AddRecord(StatRec &rec) {
	// Append new record into list
	records.push_back(new StatRec(rec));
	// Update global info
	Refresh();
}
// Free memory (remove all records)
void Statistic::Clear() {
	for(size_t i = 0; i < records.size(); i++)
		delete records.at(i);
}
// Flush into file
void Statistic::SaveStat() {
	// Open file
	ofstream outf(savePath, ios::trunc);
	// Save records (one record per line)
	for(size_t i = 0; i < records.size(); i++)
		outf << records.at(i)->recDate << '#'
		<< records.at(i)->Width << '#' << records.at(i)->Height << '#' << records.at(i)->BombCount << '#'
		<< records.at(i)->gameTime << '#' << ((records.at(i)->WinGame) ? "win" : "loose") << '#' << endl;
	// Close file
	outf.close();
}

// Read records from file
void Statistic::LoadStat() {
	// Open file
	ifstream inf(savePath);
	if(!inf) return;
	// Temp buff
	string buff = "";
	char k;
	StatRec temp;
	// Read file
	while(!inf.eof()) {
		k = inf.get();
		// End of the record (One record per line)
		if(k == '\n')
		{
			// Parse string params
			Parse(&temp, buff);
			buff.clear();
			// Append new game stat object
			records.push_back(new StatRec(temp));
		}
		buff += k;
	}
	// Update game stats
	Refresh();
	// Close file
	inf.close();
}
// Parse params from the string int Stat record
void Statistic::Parse(StatRec *p, string buff) {
	string temp = "";
	int count = 0;
	for(size_t i = 0; i < buff.size(); i++) {
		// End of the param found
		if(buff.at(i) == '#') {
			switch(count) {
				// Date of the game
			case 0:
				strcpy(p->recDate, temp.c_str());
				break;
				// Field width
			case 1:
				p->Width = atoi(temp.c_str());
				break;
				// Field height
			case 2:
				p->Height = atoi(temp.c_str());
				break;
				// Bombs count
			case 3:
				p->BombCount = atoi(temp.c_str());
				break;
				// Elapsed time
			case 4:
				p->gameTime = atoi(temp.c_str());
				break;
				// Game result: Win/Loose
			case 5:
				p->WinGame = (strcmp(temp.c_str(), "win") == 0) ? true : false;
				break;
			default:
				break;
			}
			// Clear buffer string
			temp.clear();
			// Next param
			count++;
		}
		// Append to the buffer string
		else if(buff.at(i) != '\n')
			temp += buff.at(i);
	}
}
// Update global info
void Statistic::Refresh() {
	wins = 0;
	looses = 0;
	TotalTime = 0;
	slowGame = 0;
	quickGame = MAXLONG;
	for(size_t i = 0; i < records.size(); i++) {
		// Total wins
		if(records.at(i)->WinGame) wins++;
		// Total loose
		else looses++;
		// Total spend time
		TotalTime += records.at(i)->gameTime;
		// Longest game
		if(records.at(i)->gameTime > slowGame && records.at(i)->WinGame)
			slowGame = records.at(i)->gameTime;
		// Fastest game
		if(records.at(i)->gameTime < quickGame && records.at(i)->WinGame)
			quickGame = records.at(i)->gameTime;
	}
}

// Convert all stat records to string
string Statistic::ToString() {
	char buff[32];
	string str = "\r\n\r\n";
	_itoa(wins, buff, 10);
	str += "the number of the won games:\t ";
	str += buff; str += "\r\n";
	_itoa(looses, buff, 10);
	str += "the number of the loose games:\t ";
	str += buff; str += "\r\n";
	_itoa(TotalTime, buff, 10);
	str += "Total elapsed game time:\t ";
	str += buff; str += "\r\n";
	_itoa(slowGame, buff, 10);
	str += "Longest win time:\t ";
	str += buff; str += "\r\n";
	if(quickGame == MAXLONG)
		_itoa(0, buff, 10);
	else
		_itoa(quickGame, buff, 10);
	str += "Fastest win time:\t ";
	str += buff; str += "\r\n";
	return str;
}
// Return game statistic list
vector<StatRec*> Statistic::GetRecords() { return records; }
