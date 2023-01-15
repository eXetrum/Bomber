#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
using namespace std;

// Statistic record entry
struct StatRec {
	// Default ctor
	StatRec();
	// Parametrized ctor
	StatRec(int Width, int Height, int BombCount, long gameTime, bool WinGame);
	// Copy ctor
	StatRec(const StatRec &href);
	// Convert to string
	string ToString();
	// Date and time for this record
	char recDate[80];
	// Field params
	int Width, Height, BombCount;
	// Elapsed time
	long gameTime;
	// Game result
	bool WinGame;
};

class Statistic {
private:
	string savePath;
	// Total games with "win" result
	long wins;
	// Total games with "loose" result
	long looses;
	// Total game time spend for all games
	long TotalTime;
	// Longest win time
	long slowGame;
	// Fastest win time
	long quickGame;
	// List of stat records
	vector<StatRec*> records;
	// Parse string params
	void Parse(StatRec*, string);
public:
	// Default ctor
	Statistic();
	// Dtor
	~Statistic();
	// Add stat record
	void AddRecord(StatRec&);
	// Remove all records
	void Clear();
	// Flush data to the file
	void SaveStat();
	// Read data from file
	void LoadStat();
	// Update global game info
	void Refresh();
	// Convert all records to string representation
	string ToString();
	// Get list of game stats
	vector<StatRec*> GetRecords();
};