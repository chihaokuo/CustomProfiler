// Author: Chi-Hao Kuo
// Project: Custom Instrumented Profiler
// Updated: 11/11/2015

/******************************************************************************
	Usage:
		Add "ProfileMe;" line at beginning of each function that needs
		to be profiled. The trick is to create a ProfileThis instance at the
		function scope, and call Enter()/Leave() methods within the scope.
		
	Requirement:
		Have singleton class for IProfiler.
		
******************************************************************************/
#pragma once

#include <intrin.h>
#include <windows.h>
#include <memory>
#include <string>
#include <vector>

class IProfiler;

#define ProfileMe		ProfileThis profile(__FUNCTION__, TABLE);

// Use constructor and destructor to start and end profiler use for a certain function
class ProfileThis
{
private:
	IProfiler *m_profiler_;

public:
	// Constructor.
	ProfileThis(const char *functionname);

	// Destructor.
	~ProfileThis();
};
