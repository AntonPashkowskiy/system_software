#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "LZW.h"

#define BITS_IN_CODE 12
#define TABLE_SIZE 4095
#define ACCUMULATOR_SIZE 5000

compressor::compressor()
{
	// инициализация строковой таблицы
	prefix_codes = new int[TABLE_SIZE];
	characters = new unsigned char[TABLE_SIZE];
	number_of_codes = InitializeStringTable();
	// создание буффера аккумулятора
	buffer_accumulator = new unsigned char[ ACCUMULATOR_SIZE ];
}

compressor::~compressor()
{
	delete [] prefix_codes;
	delete [] characters;
	delete [] buffer_accumulator;
}

/*
    Метод инициализации массива символов и массивов индексов
    символов предшествующих заданному.
*/
int compressor::InitializeStringTable()
{
   int i = 0;

   while( i < 256 )
   {
      prefix_codes[ i ] = -1;
      characters[ i ] = ( unsigned char )i;
      i++;
   }

   return i;
}

/*
    Метод поиска в символьной таблице заданного символа с заданным индексом предыдущего символа.
*/
int compressor::SearchInStringTable( int searching_prefix_code, unsigned char searching_character)
{
   int i = 0;
   
   while( i < number_of_codes )
   {
      	if(( prefix_codes[ i ] == searching_prefix_code) && (characters[ i ] == searching_character)) 
      	{
      		return i;
      	}

      	i++;
   }

   return -1;
}

/*
    Метод предназначенный для сжатия информации из буфера и записи этой информации в файл.
    Если используется один объект класса метод сохраняет своё состояние, и возвращается к сжатию
    при поступлении следующего буфера.
*/
int compressor::Compress( unsigned char* in_buffer, int in_buffer_size, int out_file_descriptor, bool end )
{
   int return_code = 0;
   
   // начало сжатия
   if( is_first_time )
   {
   		if( OutputCode( out_file_descriptor, -2 ) < 0 ) 
   		{   
      		return -1;
   		}

   		is_first_time = false;
   }  
   
   for( int i = 0; i < in_buffer_size; i++ ) 
   {      
   		current_character = in_buffer[ i ];
     	return_code = SearchInStringTable( current_prefix_code, current_character );

      	if( return_code >= 0 )
      	{
         	current_prefix_code = return_code;
      	} 
      	else 
      	{
         	prefix_codes[ number_of_codes ] = current_prefix_code;
         	characters[ number_of_codes ] = current_character;
         	number_of_codes ++;
         
         	if( OutputCode( out_file_descriptor, current_prefix_code ) < 0 ) 
         	{  
            	return -1;
         	}

         	current_prefix_code = ( unsigned int ) current_character;
      	}

      	if( number_of_codes >= TABLE_SIZE )
      	{
         	if( OutputCode( out_file_descriptor, current_prefix_code ) < 0) 
         	{	  
            	return -1;
         	}

         	current_prefix_code = -1;
         	// выводим код переполнения строковой таблицы
         	if( OutputCode( out_file_descriptor, TABLE_SIZE ) < 0 ) 
         	{ 
            	return -1;
         	}
         	// реинициализация строковой таблицы
         	number_of_codes = InitializeStringTable();
        }
   }

   if ( end )
   {
   	   	if( OutputCode( out_file_descriptor, current_prefix_code ) < 0 ) 
   		{   
      		return -1;
   		}
   		// записываем признак конца сжатия
   		if( OutputCode( out_file_descriptor, -1 ) < 0 ) 
   		{
      		return -1;
   		}
   }
  
   return 0;
}

/*
    Высчитывает код символа и выводит в файл.
*/
int compressor::OutputCode( int out_descriptor, int code )
{
   static unsigned char buffer;
   static int bits_in_buffer;
   int i = 0;
   int two_in_power_bit_count = 1;
   
   if( code == -2 ) 
   {
      	buffer = ( unsigned char )0;
      	bits_in_buffer = 0;
      	return 0;
   }

   if( code == -1 ) 
   {
      	if( bits_in_buffer != 0 || current_buffer_index > 0 )
        {
            if( bits_in_buffer != 0 )
            {
                Out( out_descriptor, buffer, true, true );
            }
      		else
            {
                Out( out_descriptor, buffer, false, true );
            } 
        }

      	return 0;
   }

   if( code >= 0 ) 
   {
      	while( i < bits_in_buffer )
      	{
        	two_in_power_bit_count *= 2;
        	i++;
      	}
      	i = 0;
      	while( i < BITS_IN_CODE ) 
      	{
         	buffer += two_in_power_bit_count * ( code % 2);
         	code /= 2;
         	two_in_power_bit_count *= 2;
         	bits_in_buffer++;
         	i++;

         	if( bits_in_buffer >= 8 )
         	{
         		Out( out_descriptor, buffer, true, false );
            	buffer = ( unsigned char )0;
            	bits_in_buffer = 0;
            	two_in_power_bit_count = 1;
         	}
      	}
      	return 0;
   }

   return -1;
}

/*
	Метод предназначенный для буферизированного вывода. 
	Информация записывается в файл только когда заполняется буфер или заканчивается входная инфромация.
*/
int compressor::Out( int out_descriptor, unsigned char code, bool write_last, bool end )
{
	if( current_buffer_index == ACCUMULATOR_SIZE - 1 || end )
	{
        if( write_last )
        {
            buffer_accumulator[ current_buffer_index ] = code;
            current_buffer_index ++;
        }
		
		int bytes_written = write( out_descriptor, buffer_accumulator, current_buffer_index );

		if( bytes_written != current_buffer_index )
		{
			return -1;
		}

		total_bytes_written += bytes_written;
		current_buffer_index = 0;
	}
	else
	{
		buffer_accumulator[ current_buffer_index ] = code;
		current_buffer_index ++;
	}

	return 0;
}

/*
    Метод расжатия информации из буффера длинной in_buffer_size в вектор out_vector
    При использовании одного обьекта класса compressor метод сохраняет своё состояние до
    поступления следующего буфера с информацией.
*/
int compressor::Decompress( unsigned char* in_buffer, int in_buffer_size, vector<unsigned char>& out_vector )
{
	int index = 0;
    int temp_code = 0;
    static int state = -1;
    
    if( is_first_time )
    {
    	current_code = GetCode( in_buffer, in_buffer_size, index );

    	if( OutputStringForCode( out_vector, current_code ) < 0 ) 
    	{
    		return -1;
    	}

    	old_code = current_code;
    	is_first_time = false;
    }
    
    while( index < in_buffer_size )
    {
        if( state != 2 )
        {
   		    if( (temp_code = GetCode( in_buffer, in_buffer_size, index )) == -1 )
            {
                return 0;
            }
            else
            {
                current_code = temp_code;
            }
        }
        else
        {
            state = -1;
        }

      	if( current_code == TABLE_SIZE )
      	{
        	number_of_codes = InitializeStringTable();
        	
            if( (temp_code = GetCode( in_buffer, in_buffer_size, index )) == -1 )
            {
                state = 2;
                return 0;
            }
            else
            {
                current_code = temp_code;
            }
         
         	if( current_code < 0 ) break;
         	if( OutputStringForCode( out_vector, current_code ) < 0 ) 
         	{
            	return -1;
         	}

         	old_code = current_code;
      	} 
      	else
      	{
      		if( current_code <= number_of_codes - 1 )
      		{
	            if( OutputStringForCode( out_vector, current_code ) < 0 ) 
	            { 
	               	return -1;
	            }
	            	
	            current_prefix_code = old_code;
	            current_character = GetFirstCharacterForCode( current_code );
	            prefix_codes[ number_of_codes ] = current_prefix_code;
	            characters[ number_of_codes ] = current_character;
	            number_of_codes ++;
	            old_code = current_code;
        	}
        	else
        	{
        		current_prefix_code = old_code;
            	current_character = GetFirstCharacterForCode( current_prefix_code );
            	prefix_codes[ number_of_codes ] = current_prefix_code;
            	characters[ number_of_codes ] = current_character;
            	number_of_codes ++;
            
            	if( OutputStringForCode( out_vector, number_of_codes - 1 ) < 0 ) 
            	{   
               		return -1;
            	}

            	old_code = current_code;
        	}
        }
   }
   
   return 0;
}

/*
    Метод выдаёт очередной код символа расжатой инфомации по символу сжатой информации.
*/
int compressor::GetCode( unsigned char* in_buffer, int in_buffer_size, int& index )
{
   static unsigned char buffer;
   static int number_of_bits_in_buffer = 0;
   static int cyrcle_state = -1;
   static int tip_state = -1;
   static int code_state = -1;
   int i = 0;
   int two_in_power_i = 1;
   int code = 0;
   
   while( i < BITS_IN_CODE )
   {
        if( cyrcle_state != -1 )
        {
            // восстонавливаем состояние цикла.
            i = cyrcle_state;
            two_in_power_i = tip_state;
            code = code_state;
            cyrcle_state = -1;
        }

      	if( number_of_bits_in_buffer == 0 && index < in_buffer_size )
      	{
      		buffer = in_buffer[ index ];
            index ++;
         	number_of_bits_in_buffer = 8;
      	}
        else if( number_of_bits_in_buffer == 0 && index == in_buffer_size )
        {
            // полностью сохраняем состояние цикла
            cyrcle_state = i;
            tip_state = two_in_power_i;
            code_state = code;
            return -1;
        }

      	code += ( buffer % 2 ) * two_in_power_i;
      	buffer /= 2;
      	number_of_bits_in_buffer --;
      	i ++;
      	two_in_power_i *= 2;
   }

   return code;
}

/*
    Получает самый первый символ для заданного кода.
*/
unsigned char compressor::GetFirstCharacterForCode( int current_code ) 
{
   	while( prefix_codes[ current_code ] >= 0 )
   	{
      	current_code = prefix_codes[ current_code ];
   	}
   	
   	return characters[ current_code ];
}

/*
    Записывает расжатые символы в вектор.
*/
int compressor::OutputStringForCode( vector<unsigned char>& out_vector, int current_code )
{
  	if( current_code >= 0 )
   	{
      	int code = current_code;
      	int string_length = 0;
      	int i;

     	while( prefix_codes[ code ] >= 0 ) 
     	{
         	code = prefix_codes[ code ];
         	string_length ++;
      	}
      	
      	unsigned char* output_string = new unsigned char[ string_length + 1 ];
      	code = current_code;
      	i = string_length;
      	
      	while( i >= 0 )
      	{
         	output_string[ i ] = characters[ code ];
         	code = prefix_codes[ code ];
         	i --;
      	}
      	
      	i = 0;
      
      	while( i <= string_length )
      	{
      		out_vector.push_back( output_string[ i ] );
         	i ++;
      	}
      	
      	delete [] output_string;
      	return 0 ;
   } 
   else
   {
   	  	return -1;
   } 
}

/*
    Метод получения полного размера сжатой информации.
*/
size_f compressor::GetTotalCompressedSize()
{
	return total_bytes_written;
}