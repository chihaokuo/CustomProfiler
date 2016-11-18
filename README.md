# CustomProfiler
Custom Instrumented Profiler

	Usage:
		Add "ProfileMe;" line at beginning of each function that needs
		to be profiled. The trick is to create a ProfileThis instance at the
		function scope, and call Enter()/Leave() methods within the scope.
		
	Requirement:
		Have singleton class for IProfiler.
