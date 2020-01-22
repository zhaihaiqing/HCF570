/* Includes ------------------------------------------------------------------*/
#include "main.h"

Datastorage_type 	Datastorage;
Deviceinfo_type     Deviceinfo;


/*	存储方案：
**	AT45DB041D/E默认每页264byte
**	每组数据8byte,
**	每页最后8byte不用于存储数据，每页能存储32条数据
*/


