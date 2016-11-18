// Author: Chi-Hao Kuo
// Project: Custom Instrumented Profiler
// Updated: 11/11/2015

#pragma once

#include <intrin.h>
#include <windows.h>
#include <memory>
#include <string>
#include <vector>

// function node
class PerfNode
{
public:
	std::string function_name_;							// function name as ID
	std::vector<std::shared_ptr<PerfNode>> children_;	// child node
	std::shared_ptr<PerfNode> parent_;					// parent node

	double qpcfrequency_;								// frequency used for QPC
	double qpcelapsedtime_;								// total elapsed time
	LARGE_INTEGER qpcstarttime_;						// start time
	LARGE_INTEGER qpcendtime_;							// end time

	unsigned __int64 cyclestarttime_;					// start cycle
	unsigned __int64 cycleendtime_;						// end cycle
	unsigned __int64 cycleelapsedtime_;					// total elapsed cycles

	unsigned int recursion_;							// number of recusions for function					
	unsigned int calls_;								// number of calls for function

	// Constructor.
	PerfNode(const char *name);

	// Destructor.
	~PerfNode();

	// Start the timer.
	void TimerStart(void);

	// End the timer.
	void TimerEnd(void);

	// Print out timer info to file.
	void PrintTimer(unsigned int level, std::ofstream &file);

	// Print out node to file.
	void PrintNode(unsigned int level, std::ofstream &file);
};

// profiler class
class IProfiler
{
public:
	std::vector<std::shared_ptr<PerfNode>> nodelist_;	// list of all nodes (garbage collection)
	std::shared_ptr<PerfNode> root_;					// root node
	std::shared_ptr<PerfNode> current_;					// current node
	unsigned int level_;								// current level

	// Constructor.
	IProfiler();

	// Destructor.
	~IProfiler();

	// Enter a function.
	void Enter(const char *name);

	// Leave a function.
	void Leave(void);

	// Print out the entire tree to file.
	void PrintTree(const char * filename);

	// Find certain function from children.
	std::shared_ptr<PerfNode> FindChildren(const char *name);

	// Create a new node.
	std::shared_ptr<PerfNode> CreateNode(const char *name);
};

// Return 0 if number is too small.
double NearZero(double number);
