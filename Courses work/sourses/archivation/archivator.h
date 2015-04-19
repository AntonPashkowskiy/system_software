#if !defined( ABSTRACT_ARCHIVATOR )
#define ABSTRACT_ARCHIVATOR
#include "../options/archive_options.h"
#include "title_node.h"

#if !defined( STL_VECTOR )
#define STL_VECTOR;
#include <vector>
#endif

class archivator
{
public:
	virtual void RunArchivation( archive_options* options ) = 0;
	virtual void ExtractFiles( archive_options* options ) = 0;
	virtual void RemoveFiles( archive_options* options ) = 0;
	virtual std::vector<title_node> GetTitle( archive_options* options ) = 0;
	virtual bool ChechIntegrity( archive_options* options ) = 0;
};

#endif