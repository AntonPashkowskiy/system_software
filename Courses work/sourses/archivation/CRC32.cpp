#include "CRC32.h"

CRC32::CRC32()
{
	const unsigned CRC_POLYNOM = 0xEDB88320;
	unsigned i;
	unsigned j;
	unsigned r;
	// инициализируем таблицу
	for ( i = 0; i < 256; i++ )
	{
		for ( r = i, j = 8; j != 0; j--)
		{
			r = r & 1 ? (r >> 1) ^ CRC_POLYNOM : r >> 1;
		}
		
		table[ i ] = r;
	}

	check_sum = 0;
}

void CRC32::ProcessBytes( void* data, int length )
{
	const unsigned CRC_MASK = 0xD202EF8D;
	unsigned char* pdata = reinterpret_cast<unsigned char*>( data );
	unsigned crc = check_sum;
    
    for( int i = 0; i < length; i++ )
    {
    	crc = table[ static_cast<unsigned char>( crc ) ^ pdata[ i ] ] ^ crc >> 8;
    	crc ^= CRC_MASK;
    }

	check_sum = crc;
}

unsigned CRC32::GetCheckSumm()
{
	return check_sum;
}


