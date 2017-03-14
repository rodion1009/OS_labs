#include <iostream>
#include <Windows.h>
#include <string>
#include <time.h>
using namespace std;

#define MIN_FILE_SIZE 10240
#define MAX_FILE_SIZE 10485760

void generateString(string &s, int size) {
	srand(time(NULL));
	for (int i = 0; i <= size; i++) {
		char symbol = 32 + rand() % 94;  //исключение непечатных символов таблицы ASCII
		if (symbol != '*' && symbol != '|' && symbol != '\\' && symbol != ':' && symbol != '"' && symbol != '<' && symbol != '>' && symbol != '?' && symbol != '/') {
			s += symbol;
		}
	}
}

void printFormattedTime(SYSTEMTIME time) {
	printf("%02d/%02d/%4d %02d:%02d\n",
		time.wDay,
		time.wMonth,
		time.wYear,
		time.wHour,
		time.wMinute
	);
}

void convertTime(FILETIME &time, SYSTEMTIME &sysTime) {
	FILETIME tmpTime;
	FileTimeToLocalFileTime(&time, &tmpTime);
	FileTimeToSystemTime(&tmpTime, &sysTime);
}

void printCreationTime(HANDLE h) {
	FILETIME time;
	SYSTEMTIME sysTime;
	GetFileTime(h, &time, NULL, NULL);
	convertTime(time, sysTime);
	printFormattedTime(sysTime);
}

void printLastAccessTime(HANDLE h) {
	FILETIME time;
	SYSTEMTIME sysTime;
	GetFileTime(h, NULL, &time, NULL);
	convertTime(time, sysTime);
	printFormattedTime(sysTime);
}

void printLastWriteTime(HANDLE h) {
	FILETIME time;
	SYSTEMTIME sysTime;
	GetFileTime(h, NULL, NULL, &time);
	convertTime(time, sysTime);
	printFormattedTime(sysTime);
}

bool openFile(HANDLE &h, string name) {
	h = CreateFile(TEXT(name.c_str()), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (h == INVALID_HANDLE_VALUE) {
		cout << "При создании или чтении файла произошла ошибка!" << endl;
		return false;
	} 
}


int main(void) {
	setlocale(LC_ALL, "RUS");

	//Генерация случайного имени файла
	string name;
	int nameSize = 1 + rand() % 200;
	generateString(name, nameSize);
	name += ".txt";
	
	//Создание файла
	HANDLE hfile;
	bool success = openFile(hfile, name);
	if (!success) return 0;

	//Заполнение файла строкой случайной длины
	srand(time(NULL));
	long int size = MIN_FILE_SIZE + ((rand() * 100) % (MAX_FILE_SIZE - MIN_FILE_SIZE));
	char *s = new char[size];
	LPDWORD written = NULL;
	WriteFile(hfile, s, size, written, NULL);

	CloseHandle(hfile); //Зыкрытие после создания и заполнения файла

	//Вывод времени
	success = openFile(hfile, name);
	if (!success) return 0;

	cout << "Время создания файла: ";
	printCreationTime(hfile);

	cout << "Время последнего доступа: ";
	printLastAccessTime(hfile);

	cout << "Время последней модификации: ";
	printLastWriteTime(hfile);

	//Получение атрибутов файла
	DWORD attributes = GetFileAttributes(name.c_str());
	cout << endl << "Атрибуты файла:" << endl;
	attributes & FILE_ATTRIBUTE_READONLY ? cout << "Файл в режиме read-only\n" : cout << "Файл не в режиме read-only\n";
	attributes & FILE_ATTRIBUTE_ARCHIVE ? cout << "Файл может быть архивирован\n" : cout << "Файл не может быть архивирован\n";
	attributes & FILE_ATTRIBUTE_COMPRESSED ? cout << "Файл сжат\n" : cout << "Файл не сжат\n"; 
	attributes & FILE_ATTRIBUTE_DIRECTORY ? cout << "Файл является директорией\n" : cout << "Файл не является директорией\n";
	attributes & FILE_ATTRIBUTE_ENCRYPTED ? cout << "Файл зашифрован\n" : cout << "Файл не зашифрован\n";
	attributes & FILE_ATTRIBUTE_HIDDEN ? cout << "Файл скрыт\n" : cout << "Файл не скрыт\n";
	attributes & FILE_ATTRIBUTE_TEMPORARY ? cout << "Файл является временным\n" : cout << "Файл не является временным\n";
	attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED ? cout << "Файл индексирован\n" : cout << "Файл не индексирован\n";
	attributes & FILE_ATTRIBUTE_OFFLINE ? cout << "Файл доступен автономно\n" : cout << "Файл не доступен автономно\n";
	attributes & FILE_ATTRIBUTE_SYSTEM ? cout << "Файл является системным\n" : cout << "Файл не является системным\n";
	attributes & FILE_ATTRIBUTE_VIRTUAL ? cout << "Файл зарезервирован системой\n" : cout << "Файл не зарезервирован системой\n";

	//Получение и вывод размера файла
	DWORD fileSize = GetFileSize(hfile, NULL);
	printf("\nРазмер файла: %d байт\n", fileSize);

	CloseHandle(hfile); //Зыкрытие после вывода информации о файле

	success = openFile(hfile, name);
	if (!success) return 0;

	//Изменение времени создания файла
	SYSTEMTIME st;
	st.wDay = 11;
	st.wDayOfWeek = 1;
	st.wMilliseconds = 0;
	st.wMonth = 1;
	st.wYear = 1601;
	st.wHour = 15;
	st.wMinute = 6;
	st.wSecond = 0;

	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	SetFileTime(hfile, &ft, NULL, NULL);

	cout << endl << "Изменённое время создания файла: ";
	printCreationTime(hfile);

	CloseHandle(hfile); //Закрытие файла после изменения даты создания

	success = openFile(hfile, name);
	if (!success) return 0;

	//Изменение времени модификации файла на послезавтра
	SYSTEMTIME today, newDate;
	GetSystemTime(&today);

	newDate.wYear = today.wYear;
	newDate.wMonth = today.wMonth;
	(today.wDayOfWeek + 2) > 7 ? newDate.wDayOfWeek = 7 - (today.wDayOfWeek + 2) : newDate.wDayOfWeek = today.wDayOfWeek + 2;
	newDate.wHour = 1;
	newDate.wMinute = 0;
	newDate.wSecond = 0;
	newDate.wMilliseconds = 0;

	int day = today.wDay + 2;
	switch (today.wMonth) {
	case 2:
		day -= 28;
		if (day > 0) {
			newDate.wDay = day;
			newDate.wMonth = ++today.wMonth;
		} else {
			newDate.wDay = today.wDay + 2;
		}
		break;
	case 12:
		day -= 31;
		if (day > 0) {
			newDate.wDay = day;
			newDate.wMonth = 1;
			newDate.wYear = ++today.wYear;
		} else {
			newDate.wDay = today.wDay + 2;
		}
		break;
	case 4: 
	case 6: 
	case 9:
	case 11:
		day -= 30;
		if (day > 0) {
			newDate.wDay = day;
			newDate.wMonth = ++today.wMonth;
		} else {
			newDate.wDay = today.wDay + 2;
		}
		break;
	default:
		day -= 31;
		if (day > 0) {
			newDate.wDay = day;
			newDate.wMonth = ++today.wMonth;
		} else {
			newDate.wDay = today.wDay + 2;
		}
		break;
	}

	FILETIME ftNewDate;
	SystemTimeToFileTime(&newDate, &ftNewDate);
	SetFileTime(hfile, NULL, NULL, &ftNewDate);

	cout << endl << "Изменённое время последнего доступа: ";
	printLastWriteTime(hfile);

	CloseHandle(hfile);

	system("Pause");
	return 0;
}