/** 
*  @file   NoCopyable.h
*  @author wing<totti19851101@gmail.com>
*
*  @brief  Private copy constructor and copy assignment ensure classes derived from
*		class noncopyable cannot be copied.
*  @date  2010-9-9 
*/

#pragma once

class NonCopyable
{
protected:
	NonCopyable() {}
	~NonCopyable() {}
private:
	NonCopyable(const NonCopyable&);
	const NonCopyable& operator=(const NonCopyable&);
};