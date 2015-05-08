#if !defined( CRC32_A )
#define CRC32_A

// алгоритм вычисления контрольной суммы CRC32
// инициализация таблицы происходит с помощью стандартного 
// полинома 0xEDB88320

class CRC32
{
private:
	unsigned table[ 256 ];
	unsigned check_sum = 0;
public:
	CRC32();
	unsigned GetCheckSumm();
	void ProcessBytes( void* data, int length ); 
};

#endif