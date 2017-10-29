// Client.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
#include <iostream>
#include "conio.h"
using namespace std;


#define NAME_MAX_LENGTH 30
#define BUFF_MAX_SIZE 500

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char* parseCrosswordInfo(string info, int& _numberOfPlayer, int& _turn, int& _crosswordLength)
{
	char *newInfo = new char[info.size() + 1];
	strcpy(newInfo, info.c_str());
	_numberOfPlayer = atoi(strtok(newInfo, "/"));
	_turn = atoi(strtok(NULL, "/"));
	_crosswordLength = atoi(strtok(NULL, "/"));
	return strtok(NULL, "/");
}

char* parseGuessWordPosition(char* appearance)
{
	char* result = new char[100];
	char playerName[30];
	int startIndex = 0;
	int len = strlen(appearance);

	for (int i = 2; i < len; i++)
	{
		if (appearance[i] == '/')
		{
			playerName[startIndex] = '\0';
			startIndex = i + 1;
			break;
		}
		playerName[startIndex] = appearance[i];
		startIndex++;
	}

	strcpy_s(result, sizeof(playerName), playerName);
	result = strcat(result, " vua doan ky tu '");
	result = strcat(result, &appearance[len - 1]);


	int sum = 0;
	result = strcat(result, "'. Vi tri:");

	//Bo 'A' va '/' va ky tu cuoi 
	for (int i = startIndex; i < len - 1; i++)
	{
		if (appearance[i] == '/')
		{
			char c_sum[10];

			_itoa_s(sum, c_sum, 10);
			result = strcat(result, " ");
			result = strcat(result, c_sum);
			sum = 0;
		}
		else
			sum = sum * 10 + appearance[i] - '0';
	}


	return result;

}

char* parseGuessString(char* appearance)
{
	//Duyet tu ten nguoi choi
	char name[10];
	int index = 0;
	int len = strlen(appearance);
	for (int i = 2; i < len; i++)
	{
		if (appearance[i] == '/')
		{
			name[index] = '\0';
			index = i + 1;
			break;
		}
		else
		{
			name[index] = appearance[i];
			index++;
		}
	}

	char* result = new char[100];
	strcpy_s(result, sizeof(name), name);
	result = strcat(result, " doan dap an la: '");
	result = strcat(result, (appearance + index * sizeof(char)));
	result = strcat(result, "' nhung khong chinh xac!");
	return result;
}

bool isValidName(char* _name)
{
	if (strlen(_name) == 0)
	{
		return false;
	}

	if(strlen(_name)>8)
	{
		return false;
	}

	for (unsigned int i = 0; i < strlen(_name); i++)
	{
		if (!((_name[i] >= 'a' && _name[i] <= 'z') || (_name[i] >= 'A' && _name[i] <= 'Z') || (_name[i] >= '0' && _name[i] <= '9')))
		{
			//Ten chi duoc cau thanh tu nhung chu cai 'a' ... 'z' hoac 'A' ... 'Z' hoac '0' ... '9'
			return false;
		}
	}

	return true;
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
			cout << "\t\t\t\t\t      ENGLISH GAME" << endl << endl;


			//////////////////////////////////////////////////////////////////////////
			//Khai bao
			//////////////////////////////////////////////////////////////////////////
			//Khoi tao bien socket cua client
			CSocket client;
			//char* clientName;
			int turn = 0;
			int numberOfPlayer = 0;
			int crosswordLength = 0;
			int point;
			int tmpLen;
			char *title;
			char temp[BUFF_MAX_SIZE];


			//////////////////////////////////////////////////////////////////////////
			//Khoi tao
			//////////////////////////////////////////////////////////////////////////
			AfxSocketInit(NULL);
			client.Create();
			
			srand(static_cast<unsigned int>(time(NULL))); //Srand som de lay time khac nhau giua cac client


			//////////////////////////////////////////////////////////////////////////
			//Connect toi server
			//////////////////////////////////////////////////////////////////////////
			if (client.Connect(_T("127.0.0.1"), 8800))
			{
				//Nhap den khi nao hop le
				do 
				{
					cout << " --> Nhap ID: ";
					gets_s(temp);
					
					while (!isValidName(temp))
					{
						cout << "  - EnglishGame: Khong hop le! ID chi gom cac ky tu 'a' .. 'z', 'A' .. 'Z', '0' .. '9' va duoi 8 ki tu.\n";
						cout << "  - Nhap lai: ";
						gets_s(temp);
					}
					
					int nameLength = strlen(temp);
					temp[nameLength] = 0;

					client.Send(temp, nameLength, 0);
					
					tmpLen = client.Receive(temp, 10);
					temp[tmpLen] = '\0';

					if (strcmp(temp, "0") != 0)
					{
						break;
					}
					cout << "  - EnglishGame: ID nguoi choi da ton tai!" << endl;
				} 
				while (true);
				
				//Dang ky voi server thanh cong
				turn = atoi(temp);

				cout << "  - EnglishGame: Ket noi den server thanh cong!" << endl;

				//Khoi tao diem
				client.Receive(temp, 10);
				point = atoi(temp);
				if (point == 3)
				{
					client.Send("ok", sizeof("ok"));
					cout << "  - EnglishGame: Server khoi tao diem: " << point << endl;
				}
				else
					client.Send("error", sizeof("error"));
				

				//Nhan thong tin cua de
				tmpLen = client.Receive(temp, BUFF_MAX_SIZE,0);
				temp[tmpLen] = '\0';
				
				if (temp != NULL && tmpLen > 0)
				{
					client.Send("ok", sizeof("ok"));
				}
				else client.Send("error", sizeof("error"));

				title = parseCrosswordInfo(temp, numberOfPlayer, turn, crosswordLength);

				//In thong tin vong choi
				cout << endl;
				cout << "\t\t\t\t\t     THONG TIN CUA VONG CHOI NAY" << endl;
				cout << "  - So nguoi choi: " << numberOfPlayer << endl;
				cout << "  - Luot choi cua ban: " << turn << endl;
				cout << "  - De bai: " << title << endl;

				//////////////////////////////////////////////////////////////////////////
				//Bat dau game
				//////////////////////////////////////////////////////////////////////////
				cout << "\t\t\t\t\t     GAME START!" << endl;
				
				int rollPoint = 0; //Bien chua diem D
				while (true)
				{
					//Nhan thong tin tu server xem minh phai lam gi
					tmpLen = client.Receive(temp, BUFF_MAX_SIZE);
					temp[tmpLen] = '\0';

					//////////////////////////////////////////////////////////////////////////
					//cout << "Bat dau lai vong while: ";
					//puts(temp);
					
					cout << endl;
					
					//Neu den luot choi
					if (strcmp(temp, "turn") == 0)
					{
						rollPoint = 1;
						
						char c_rollPoint[10];
						
						//Convert sang char*
						_itoa_s(rollPoint, c_rollPoint, 10);
						//Gui ve server
						client.Send(c_rollPoint, strlen(c_rollPoint) + 1); //Vua them +1 o day

						continue;
					}
					else if (strcmp(temp, "guess") == 0) //Den luot doan
					{
						char guess[BUFF_MAX_SIZE];
						do 
						{
							cout << " --> Nhap dap an muon doan: ";
							gets_s(guess);
						} while (strlen(guess) == 0 || guess == NULL);

						cout << endl;

						//Gui dap an vua doan ve server
						client.Send(guess, strlen(guess));

						//Cho nhan thong tin tra ve, khoi tao bien guessResponse
						char guessResponse[BUFF_MAX_SIZE];
						tmpLen = client.Receive(guessResponse, BUFF_MAX_SIZE);
						guessResponse[tmpLen] = '\0';


						if (strcmp(guessResponse, "WIN") != 0 &&  guessResponse[0] == 'W')
						{
							//Doan sai 
							point -= 1;
							cout << "  - EnglishGame: Ban doan sai! So diem hien tai: " << point << endl;
							client.Send("ok", sizeof("ok"));
							continue;
						}
						else if (strcmp(guessResponse, "WIN") == 0) //Doan dung dap an
						{
							//Win 
							point += rollPoint;
							cout << "  - EnglishGame: Ban da doan dung dap an. So diem hien tai: " << point << endl;
							goto EndGame;
						}
						else
							cout << "  ***SERVER ERROR*** Gui guess ma chua nhan duoc phan hoi!" << endl;


					}
					else if (strcmp(temp, "WIN") != 0 && temp[0] == 'W')
					{
						//Doan sai dap an
						cout << "  - EnglishGame: Nguoi choi ";
						puts(parseGuessString(temp));
						client.Send("ok", sizeof("ok")); // gui thong bao da nhan
					}
					else if (temp == NULL && tmpLen <= 0)
					{
						client.Send("error", sizeof("error"));
						continue;
					}
					else if (strcmp(temp,"WIN") == 0)
					{
						//Dap an da duoc 1 ng choi khac doan. Ket thuc game
						cout << "  - EnglishGame: Dap an da duoc giai. Tro choi ket thuc!" << endl;
						goto EndGame;
					}
					else if (strcmp(temp, "LIMIT") == 0)
					{
						//Da het luot doan
						cout << "  - EnglishGame: So luot doan vuot qua gioi han! Tro choi ket thuc!" << endl;
						goto EndGame;

					}
				}

				//Label endgame
			EndGame:
				tmpLen = client.Receive(temp, 10);
				temp[tmpLen] = '\0';

				//Neu server yeu cau gui diem
				if (strcmp(temp,"POINT") == 0)
				{
					char c_Point[10];
					_itoa(point, c_Point, 10);
					client.Send(c_Point, strlen(c_Point));
				}

				//Tong ket diem
				cout << "  - EnglishGame: So diem cua ban: " << point << endl << endl;

				//Dong client
				client.Close();
				system("pause");
			}
			else
			{
				cout << "  - EnglishGame: Khong the ket noi den server!";
				system("pause");
			}
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