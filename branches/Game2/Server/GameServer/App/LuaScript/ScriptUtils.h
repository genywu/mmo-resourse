///
/// @file ScriptUtils.h
/// @brief Some util functions for scripts.
///
#ifndef ___SCRIPT_UTILS_H_
#define ___SCRIPT_UTILS_H_

namespace ScriptUtils
{
	void UpdateValToWS( const char *varName, double val, long pos = 0 );

	void UpdateValToWS( const char *varName, const tString &val, long pos = 0 );

	void UpdateValToWS( const char *varName, const CGUID &val, long pos = 0 );
}

#endif
