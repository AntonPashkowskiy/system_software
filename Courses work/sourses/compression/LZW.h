#if !defined( LZW )
#define LZW

typedef unsigned long long size_f;

#if !defined( STD_VECTOR )
#define STD_VECTOR
#include <vector>
#endif

using namespace std;

class compressor
{
private:
	int* prefix_codes = nullptr;
	unsigned char* characters = nullptr;
	unsigned char current_character;
	int current_prefix_code = -1;
	int number_of_codes = 0;
	int current_code = 0;
	int old_code = 0;
	bool is_first_time = true;
	// буферизация вывода
	unsigned char* buffer_accumulator = nullptr;
	int current_buffer_index = 0;
	size_f total_bytes_written = 0;

	int InitializeStringTable();
	int SearchInStringTable( int searching_prefix_code, unsigned char searching_character);
	int OutputCode( int out_descriptor, int code );
	int GetCode( unsigned char* in_buffer, unsigned int in_buffer_size, unsigned int& index );
	int OutputStringForCode( vector<unsigned char>& out_vector, int current_code );
	int Out( int out_descriptor, unsigned char code, bool write_last, bool end );
	unsigned char GetFirstCharacterForCode( int current_code );
public:
	compressor();
	~compressor();
	int Compress( unsigned char* in_buffer, int in_buffer_size, int out_file_descriptor, bool end );
	int Decompress(unsigned char* in_buffer, unsigned int in_buffer_size, vector<unsigned char>& out_vector);
	size_f GetTotalCompressedSize();
};

#endif