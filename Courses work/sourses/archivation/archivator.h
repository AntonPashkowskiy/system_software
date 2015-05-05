#if !defined( ABSTRACT_ARCHIVATOR )
#define ABSTRACT_ARCHIVATOR
#include "../options/archive_options.h"
#include "title.h"

class archivator
{
public:
	virtual void RunArchivation( archive_options* options ) = 0;
	virtual void ExtractFiles( archive_options* options ) = 0;
	virtual void RemoveFiles( archive_options* options ) = 0;
	virtual archive_title GetTitle( archive_options* options ) = 0;
	virtual bool CheckIntegrity( archive_options* options ) = 0;
};

#endif