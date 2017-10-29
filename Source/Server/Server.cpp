// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include <iostream>
#include <string>
#include <vector>
#include "conio.h"
#include <fstream>
using namespace std;

//Macro define
#define NAME_MAX_LENGTH 30
#define BUFF_MAX_SIZE 500
#define MAX_GUESS_TIME 3
#define DATA_FILE_PATH "EnglishGame.txt"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Khai bao class Crossword
class Crossword
{
public:
	Crossword()
	{
		crossword = "";
		hint = "";
	}

	Crossword(string _crossword, string _hint)
	{
		this->crossword = _crossword;
		this->hint = _hint;
	}
	string crossword;
	string hint;

	string getCrossword()
	{
		return this->crossword;
	}

	string getHint()
	{

		return this->hint;
	}

	int getLength()
	{
		return crossword.length();
	}
};


vector<Crossword> crosswordFileInput(string _path)
{
	ifstream inFile;
	inFile.open(_path);
	vector<Crossword> _arr;
	if (inFile.is_open())
	{
		while (!inFile.eof())
		{
			string line;
			Crossword cw;
			if (getline(inFile, line))
			{
				cw.crossword = line;

				if (getline(inFile, line))
				{
					cw.hint = line;
				}

				/*if (getline(inFile, line))
				{
					cw.guessLimit = stoi(line);
				}*/

				_arr.push_back(cw);
			}
			else
				cout << "Loi: File du lieu sai format!" << endl;
		}

		return _arr;
		inFile.close();
	}

	cout << "Error opening file! " << endl;
	return{};
}

//Kiem tra ID dang nhap co hop le hay khong
bool isValidName(char* _name, vector<char*> _listClientsName)
{
	//Kiem tra ID co null hay khong, hay dat ID rong
	if (_name == NULL || strcmp(_name, " ") == 0 || strcmp(_name, " ") == 0)
	{
		return false;
	}

	//Duyet qua vector chua ID
	for (unsigned int i = 0; i < _listClientsName.size(); i++)
	{
		//Neu phat hien trung, tra ve false
		if (strcmp(_name, _listClientsName[i]) == 0)
		{
			return false;
		}
	}
	return true;
}

Crossword getCrossWord(vector<Crossword> _arr)
{
	if (_arr.size() == 0)
	{
		cout << "Khong tim thay de! Dang thu lay lai du lieu..." << endl;
		_arr = crosswordFileInput(DATA_FILE_PATH);

		if (_arr.size() == 0)
		{
			cout << "Lay lai du lieu khong thanh cong! Thoat game..." << endl;
			system("pause");
			exit(0);
		}
	}

	//Da co du lieu
	//Tien hanh random lay 1 de
	srand(static_cast<unsigned int>(time(NULL)));
	int index = rand() % _arr.size();

	return _arr[index];
}

string crosswordInfoConstruct(int _numberOfPlayer, int _turn ,int _crosswordLength, const char* crossword)
{
	string temp = to_string(_numberOfPlayer) + "/" + to_string(_turn) + "/" + to_string(_crosswordLength) + "/" + string(crossword);

	return temp;
}

bool checkWin(int* _guessed, int _crosswordLength)
{
	for (int i = 0; i < _crosswordLength; i++)
	{
		if (_guessed[i] == 0)
		{
			//Co de chua dc doan
			return false; 
		}
	}
	return true;
}

bool checkGuessLimitExceeded(int _guess, int _maxTimes)
{
	if (_guess >= _maxTimes)
	{
		return true;
	}
	return false;
}


char* printScoreTable(int* _array, int n, vector<char*> _listClientsName)
{
	char* winner = nullptr;
	int _count = 0;
	while (_count < n)
	{
		int _max = _array[0];
		int _maxIndex = 0;
		for (int i = 1; i < sizeof(_array); i++)
		{
			if (_array[i] > _max)
			{
				_max = _array[i];
				_maxIndex = i;
			}
		}
		//Tim ra duoc max
	
		if (_count == 0)
		{
			winner = static_cast<char*>(malloc(strlen(_listClientsName[_maxIndex]) + 1));
			strcpy(winner, _listClientsName[_maxIndex]);
		}
		cout << _count+1<< " . " << _listClientsName[_count] << ": " << _max << " diem." << endl;
		_count++;
		//Gan max = 0
		_array[_maxIndex] = 0;
	}

	return winner;
}


bool isValidNumberOfPlayer(int number)
{
	if (number > 10 || number <0)
		return false;
	if (number == NULL)
		return false;

	return true;
}

void printRoundInfomation(string _crossword, int _numberOfPlayer, string _hint, int _maxGuessLimit)
{
	cout << endl;
	cout << "\t\t\t\t\t     THONG TIN CUA VONG CHOI" << endl;
	cout << "  - De bai: " << _crossword << endl;
	cout << "  - So nguoi choi: " << _numberOfPlayer << endl;
	cout << "  - Dap an: ";
	puts(_hint.c_str());
	cout << "  - So luot doan sai toi da: " << _maxGuessLimit << endl;
}


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			//////////////////////////////////////////////////////////////////////////
			//Credits
			//////////////////////////////////////////////////////////////////////////

			cout << "\t\t\t\t\t        BO MON MANG MAY TINH" << endl;
			cout << "\t\t\t\t\t ----- DO AN LAP TRINH SOCKET ----- " << endl;
			cout << "\t\t\t\t\t ---------- ENGLISH GAME ---------- " << endl << endl;
			cout << "Thong tin sinh vien: Tran Minh Dat - 1512103" << endl;			

			//////////////////////////////////////////////////////////////////////////
			//Khai bao
			//////////////////////////////////////////////////////////////////////////

			//Khai bao cac bien socket
			CSocket server;
			CSocket* listClient;
			//Khai bao danh sach cac dap an
			vector<Crossword> listCrossword;
			//Khai bao bien
			int numberOfPlayer = 3;
			int tmpLen;
			int* guessed;
			bool isWin = false;
			bool isExceedGuessLimit = false;
			int maxGuessTimes = MAX_GUESS_TIME;
			int* numberOfGuessedTimes;
			//Khai bao bien dap an
			Crossword crossword;
			string c_Crossword, c_Hint;
			char temp[BUFF_MAX_SIZE]; //Bien tam de gui va nhan thong tin trong chuong trinh


			//////////////////////////////////////////////////////////////////////////
			//Khoi tao
			//////////////////////////////////////////////////////////////////////////

			//Lay so luong nguoi choi
			
			cout << "\n\n  --> Nhap so luong nguoi choi: ";
			cin >> numberOfPlayer;
			while (!isValidNumberOfPlayer(numberOfPlayer))
			{
				cout << "  - So nguoi choi nhap khong hop le! Chi cho phep toi da 10 nguoi choi" << endl;
				cout << "  - Nhap lai: ";
				cin >> numberOfPlayer;
			}

			//Khoi tao Windows Socket voi tham so lpwsaData bang NULL
			AfxSocketInit(NULL);
			//Tao socket voi PORT bang 7765
			server.Create(8800); //mac dinh nSocketType = 1 = SOCK_STREAM = TCP
			
			cout << "  - Server: Khoi tao server thanh cong! Dang lang nghe ket noi tu Clients..." << endl;
			server.Listen(10); //backlog = 10, chap nhan 10 ket noi dang cho ma chua dc accept
			
			listClient = new CSocket[numberOfPlayer];
			vector<char*> listClientsName(numberOfPlayer, "na");
			

			//Cho va chap nhan ket noi tu client, du N nguoi choi moi bat dau choi
			for (int i = 0; i < numberOfPlayer; i++)
			{
				//Chap nhan ket noi tu client thu [i], kieu CAsyncSocket&
				server.Accept(listClient[i]);
				
				//Kiem tra den khi nao nhan duoc ten hop le (khong trung)
				while (true)
				{
					//Nhan ten tu client gui ve
					tmpLen = listClient[i].Receive(temp, NAME_MAX_LENGTH); //Default flag = 0
					temp[tmpLen] = '\0';

					if (isValidName(temp, listClientsName))
					{
						//Ten hop le
						listClientsName[i] = new char[strlen(temp)];
						strcpy_s(listClientsName[i], sizeof(temp), temp);
						
						cout << "  - Server: Nguoi choi " << listClientsName[i] << " vua ket noi!" << endl;

						//Chep so thu tu vao bien tam de gui cho client
						//Vua de xac nhan viec dang ky thanh cong, vua de xac dinh thu tu luot choi cua client
						_itoa_s(i + 1, temp, 10);
						listClient[i].Send(temp, strlen(temp) + 1);
						break;
					}
					else
					{
						//Ten khong hop le, gui tin hieu phan hoi ve client
						//strcpy_s(temp, sizeof("0"), "0");
						listClient[i].Send("0", strlen("0")); 
						continue;
					}
				}
			}

			//Khoi tao diem va gui cho nguoi choi
			for (int i = 0; i < numberOfPlayer; i++)
			{

				char ack[BUFF_MAX_SIZE];
				listClient[i].Send("3", strlen("3")); //Khoi tao diem
				tmpLen = listClient[i].Receive(ack, BUFF_MAX_SIZE);
				ack[tmpLen] = '\0';

				if (strcmp(ack, "ok") != 0)
				{
					cout << "  ***CLIENT ERROR*** Client " << i << " nhan that bai!" << endl;
				}


			}

			
			//Lay de tu file
			listCrossword = crosswordFileInput(DATA_FILE_PATH);
			crossword = getCrossWord(listCrossword);
			//Gan cho bien chuoi dap an
			c_Crossword = crossword.getCrossword();
			c_Hint = crossword.getHint();
			
			//In thong tin cua vong choi
			printRoundInfomation(c_Crossword,numberOfPlayer, c_Hint, MAX_GUESS_TIME);
			
			//ghep thong tin de va gui cho cac client
			for (int i = 0; i < numberOfPlayer; i++)
			{
				char ack[BUFF_MAX_SIZE];
				string info = crosswordInfoConstruct(numberOfPlayer, i + 1, crossword.getLength(), c_Crossword.c_str());
				//cout << info << " " << strlen(info) << endl;
				listClient[i].Send(info.c_str(), strlen(info.c_str()));
				tmpLen = listClient[i].Receive(ack, BUFF_MAX_SIZE);
				ack[tmpLen] = '\0';

				if (strcmp(ack, "ok") != 0)
				{
					cout << "  ***CLIENT ERROR*** Client " << i << " nhan that bai!" << endl;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//Bat dau tro choi
			//////////////////////////////////////////////////////////////////////////
			cout << endl;
			cout << "\t\t\t\t\t     GAME START!" << endl;

			//Khoi tao gia tri
			numberOfGuessedTimes = new int[numberOfPlayer];
			isExceedGuessLimit = false;
			isWin = false;
			guessed = new int[strlen(c_Crossword.c_str())];
			for (unsigned int index = 0; index < strlen(c_Crossword.c_str()); index++)
			{
				guessed[index] = 0;
			}

			for (int index = 0; index < numberOfPlayer; index++)
			{
				numberOfGuessedTimes[index] = 0;
			}

			//////////////////////////////////////////////////////////////////////////
			//Game loop
			//////////////////////////////////////////////////////////////////////////
			while (true)
			{
				//Duyet lan luot qua tung nguoi choi
				for (int i = 0; i < numberOfPlayer; i++)
				{
					//Kiem tra dap an da duoc doan het chua, hoac da ai doan dung dap an chua
					if (checkWin(guessed, strlen(c_Crossword.c_str())))
					{
						//Dap an da duoc doan
						cout << "  - Server: Dap an da duoc doan! Dap an: " << c_Hint << endl;

						for (int m = 0; m < numberOfPlayer; m++)
						{
							listClient[m].Send("WIN", strlen("WIN"));
						}

						//Goto
						isWin = true;
						goto EndGame;
					}
					else if (checkGuessLimitExceeded(numberOfGuessedTimes[i], maxGuessTimes)) //Kiem tra co vuot qua so lan doan chua
					{
						//Gui thong bao toi cac client la da het luot choi, endgame
						for (int m = 0; m < numberOfPlayer; m++)
						{
							listClient[m].Send("LIMIT", strlen("LIMIT"));
						}

						//Goto
						isExceedGuessLimit = true;
						goto EndGame;
					}

					//Khoi tao bien chua diem
					char rollPoint[10];

					//////////////////////////////////////////////////////////////////////////
					//Gui luot choi cho client
					//////////////////////////////////////////////////////////////////////////
					listClient[i].Send("turn", strlen("turn"));

					//Nhan thong tin tra ve la diem ma nguoi choi quay duoc
					tmpLen = listClient[i].Receive(rollPoint, 10);
					rollPoint[tmpLen] = '\0';


					//////////////////////////////////////////////////////////////////////////
					//Gui tin hieu cho phep nguoi choi doan chu
					//////////////////////////////////////////////////////////////////////////
					listClient[i].Send("guess", strlen("guess"));
					char guess[BUFF_MAX_SIZE];

					//Nhan dap an ma nguoi choi gui ve
					tmpLen = listClient[i].Receive(guess, BUFF_MAX_SIZE);
					guess[tmpLen] = '\0';

					//put cai guess ra console kiem tra
					//puts(guess);
					
					
					//Doan dap an
					if (strcmp(guess, c_Hint.c_str()) == 0)
					{
						//Doan dung dap an
						cout << "  - Server: Nguoi choi doan dung dap an!" << endl;

						//Gui tin hieu doan dung dap an cho tat ca nguoi choi, tro choi ket thuc
						for (int m = 0; m < numberOfPlayer; m++)
						{
							listClient[m].Send("WIN", strlen("WIN"));
						}
						isWin = true;

						goto EndGame;
					}
					else
					{
						//Doan sai dap an
						//Gep chuoi thong bao nguoi choi doan sai
						//Chuoi dang "W/name/guess_cross_word"
						numberOfGuessedTimes[i] += 1;
						char guessResponse[BUFF_MAX_SIZE];
						strcpy_s(guessResponse, sizeof("W"), "W");
						strcat(guessResponse, "/");
						strcat(guessResponse, listClientsName[i]);
						strcat(guessResponse, "/");
						strcat(guessResponse, guess);
						//puts(guessResponse);

						//Gui thong tin cho tat ca cac client
						for (int m = 0; m < numberOfPlayer; m++)
						{
							listClient[m].Send(guessResponse, strlen(guessResponse));

							char ack[BUFF_MAX_SIZE];
							tmpLen = listClient[m].Receive(ack, BUFF_MAX_SIZE);
							ack[tmpLen] = '\0';

							if (strcmp(ack, "ok") != 0)
							{
								cout << "  ***CLIENT ERROR*** Client " << m << " nhan that bai!" << endl;
							}
						}
					}
				}

				//Den day se lap lai nguoi choi dau tien
			}

			//Lable endgame
		EndGame:
			if (isWin)
			{
				cout << "  - Server: Dap an da duoc giai!" << endl;
			}
			else if (isExceedGuessLimit)
			{
				cout << "  - Server: Vuot qua so lan doan! Tro choi ket thuc!" << endl;
			}

			int * listClientPoint = new int[numberOfPlayer];

			//Gui yeu cau lay diem, sau do nhan diem tu client
			for (int i = 0; i < numberOfPlayer; i++)
			{
				char c_point[10];
				listClient[i].Send("POINT", strlen("POINT")); //GUI YEU CAU
				tmpLen = listClient[i].Receive(c_point, BUFF_MAX_SIZE);
				//c_point[tmpLen] = '\0';

				listClientPoint[i] = atoi(c_point);
			}

			//In bang xep hang
			cout << endl << "\t\t\t\t\t     BANG XEP HANG" << endl;
			char* winner = printScoreTable(listClientPoint,numberOfPlayer,listClientsName);

			cout << "  - Server: Chuc mung chien thang cua " << winner << endl;
			//puts(winner);
			delete[] winner;
			delete[] listClientPoint;

			//Dong tat ca cac client va server
			for (int i = 0; i < numberOfPlayer; i++)
			{
				listClient[i].Close();
			}
			server.Close();

			system("pause");

		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}