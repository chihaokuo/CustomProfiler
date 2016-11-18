// Author: Chi-Hao Kuo
// Project: Custom Instrumented Profiler
// Updated: 11/11/2015

#include "Profiler.h"

#define PROFILER_ON

/*--------------------------------------------------------------------------*
Name:           ProfileThis

Description:    Constructor.

Arguments:      functionname:		function name to profile.

Returns:        None.
*---------------------------------------------------------------------------*/
ProfileThis::ProfileThis(const char *functionname)
{
#ifdef PROFILER_ON
	m_profiler_ = Singleton<IProfiler>::GetInstance();
	m_profiler_->Enter(functionname);
#endif
}

/*--------------------------------------------------------------------------*
Name:           ~ProfileThis

Description:    Destructor.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
ProfileThis::~ProfileThis()
{
#ifdef PROFILER_ON
	m_profiler_->Leave();
#endif
}
