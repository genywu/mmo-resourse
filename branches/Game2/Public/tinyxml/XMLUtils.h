///
/// @file XMLUtils.h
/// @brief Provide some macros to query attributes by tinyxml.
/// @author Kevin Lynx
///
#ifndef ___XML_UTILS_H_
#define ___XML_UTILS_H_

#define XML_QUERY_NUM( name, var, elem, type ) \
	{ \
		double v = 0; \
		elem->Attribute( name, &v ); \
		var = static_cast<type>( v ); \
	}
#define XML_QUERY_BOOL( name, var, elem ) \
	{ \
		double v = 0; \
		elem->Attribute( name, &v ); \
		var = (v != 0); \
	}
#define XML_QUERY_NUM_DEFAULT( name, var, elem, type, def_v ) \
	{ \
		double v = 0; \
		elem->Attribute( name, &v ); \
		var = static_cast<type>( v ); \
		var = var == type() ? def_v : var; \
	}
/// var must a c pointer or std::string
#define XML_QUERY_STR( name, var, elem ) \
	{ \
		var = elem->Attribute( name ); \
	}

#define XML_QUERY_STR_DEFAULT( name, var, elem, def_v ) \
	{ \
		const char *t = elem->Attribute( name ); \
		var = t == 0 ? def_v : t; \
	}

/// var must be a char array
#define XML_QUERY_STR_ARRAY( name, var, elem ) \
    { \
        const char *s = elem->Attribute( name ); \
        strcpy( var, s );  \
    }

#endif

