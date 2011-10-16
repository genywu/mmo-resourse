///
/// @file TypeObject.h
/// @author Kevin Lynx
/// @brief A TypeObject is an object with a type(also known as an ID) to identify it.
///
#ifndef ___TYPE_OBJECT_H_
#define ___TYPE_OBJECT_H_

namespace StateAI
{
	template <typename Tp>
	class TypeObject
	{
	public:
		TypeObject( Tp type ) : m_Type( type )
		{
		}

		/// Query the type of this object.
		Tp Type() const
		{
			return m_Type;
		}
	private:
		Tp m_Type;
	};
}

#endif
