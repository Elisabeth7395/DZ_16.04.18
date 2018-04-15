#include <iostream>
#include <conio.h>
#include <fstream> //работа с файлами

using namespace std;

bool true_png(char* format)
{
	char numbers[8] = { 0x89, 0x50, 0x4E, 0x47, 
		               0x0D, 0x0A, 0x1A, 0x0A };
	bool valid = true;
	for (int i(0); i < 8; i++)
	{
		if (!(numbers[i] == format[i]))
			valid = false;
	}
	return valid;
}

bool true_IHDR(char* check_IHDR)
{
	char numbers[4] = { 0x49, 0x48, 0x44, 0x52 };
	bool valid(1);
	for (int i(0); i < 4; i++)
	{
		if (!(numbers[i] == check_IHDR[i]))
			valid = 0;
	}
	return valid;
}

bool true_IEND(char* check_IEND)
{
	char numbers[12] = { 0x00, 0x00, 0x00, 0x00, 
		                0x49, 0x45, 0x4E, 0x44, 
		                0xAE, 0x42, 0x60, 0x82 };
	bool valid(1);
	for (int i(0); i < 12; i++)
	{
		if (!(numbers[i] == check_IEND[i]))
			valid = 0;
	}
	return valid;
}

bool true_IDAT(char* check_IDAT)
{
	char numbers[4] = { 0x49, 0x44, 0x41, 0x54 };
	bool valid(1);
	for (int i(0); i < 4; i++)
	{
		if (!(numbers[i] == check_IDAT[i]))
			valid = 0;
	}
	return valid;
}

int main(int argc, char* argv[])
{
	setlocale(0, "");

	if (argc != 2)
	{
		cout << "Неверное количество аргументов";
		_getch();
		return 0;
	}

	ifstream file;
	file.open(argv[1], ios::binary);
	if (!file.is_open())
	{
		cout << "Ошибка при открытии файла" << endl;
		_getch();
		return 0;
	}

	char format[8];
	file.read(format, 8);
	if(!true_png(format))
	{
		cout << "Выбранный файл не является файлом формата .png" << endl;
		_getch();
		return 0;
	}

	char check_l_IHDR[4];
	file.read(check_l_IHDR, 4);
	int length=_byteswap_ulong(*((unsigned int*)check_l_IHDR));

	char check_IHDR[4];
	file.read(check_IHDR, 4);
	if (!true_IHDR(check_IHDR))
	{
		cout << "Не найден IHDR" << endl;
		_getch();
		return 0;
	}
	
	file.seekg(length, ios_base::cur);

	char check_crc_IHDR[4];
	file.read(check_crc_IHDR, 4);
	int crc_IHDR = _byteswap_ulong(*((unsigned int*)check_crc_IHDR));

	file.seekg(-0xC, ios_base::end);
	char check_IEND[12];
	file.read(check_IEND, 12);
	if (!true_IEND(check_IEND))
	{
		cout << "Не найден IEND" << endl;
		_getch();
		return 0;
	}

	file.seekg(8+8+length+4+4, ios_base::beg); //Проверка наличия первого IDAT
	char check_IDAT[4];
	file.read(check_IDAT, 4);
	if (!true_IDAT(check_IDAT))
	{
		cout << "Не найден IDAT" << endl;
		_getch();
		return 0;
	}
	
	cout << "IHDR" << endl << "Длина: " << length << endl << "CRC: 0x" << hex << crc_IHDR << endl << endl;

	do {
		file.seekg(-8, ios_base::cur);
		cout << "Найден IDAT на " << dec << file.tellg() << " позиции" << endl;
		char check_l_IDAT[4];
		file.read(check_l_IDAT, 4);
		int length_idat = _byteswap_ulong(*((unsigned int*)check_l_IDAT));
		file.seekg(4 + length_idat, ios_base::cur);
		char check_crc_IDAT[4];
		file.read(check_crc_IDAT, 4);
		int crc_IDAT = _byteswap_ulong(*((unsigned int*)check_crc_IDAT));
		cout << "Длина: " << length_idat << endl << "CRC: 0x" << hex << crc_IDAT << endl << endl;

		file.seekg(4, ios_base::cur);
		check_IDAT[4];
		file.read(check_IDAT, 4);
	} while (true_IDAT(check_IDAT));

	cout << "IEND" << endl;

	_getch();
	return 0;
}