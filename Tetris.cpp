#include <iostream>
#include <string>
#include <Windows.h>
#include <thread>
#include <stdlib.h> 
#include <time.h> 
#include <vector>
#include <stdio.h>
using namespace std;

int FieldWidth = 12;			
int FieldHeight = 18;			
char *Field = nullptr; 		

string tetromino[7];			

int Rotate(int x, int y, int r)
{
	int pi = 0;
	switch (r % 4)
	{
	case 0: 
		pi = y * 4 + x;
		break;						
		

	case 1: 
		pi = 12 + y - (x * 4);	
		break;						
									

	case 2: 
		pi = 15 - (y * 4) - x;
		break;				
							

	case 3:
		pi = 3 - y + (x * 4);
		break;				
	}						

	return pi;
}

bool DoesPieceFit(int Tetromino, int Rotation, int PosX, int PosY)
{
	for (int x = 0; x < 4; x++)
		for (int y = 0; y < 4; y++)
		{

			int pi = Rotate(x, y, Rotation);

			int fi = (PosY + y) * FieldWidth + (PosX + x);
			if (PosX + x >= 0 && PosX + x < FieldWidth)
			{
				if (PosY + y >= 0 && PosY + y < FieldHeight)
				{
					if (tetromino[Tetromino][pi] != L'.' && Field[fi] != 0)
						return false;
				}
			}
		}
	return true;
}

int main()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	int ScreenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;			
	int ScreenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;			
	
	tetromino[0].append("..X.");
	tetromino[0].append("..X.");
	tetromino[0].append("..X.");
	tetromino[0].append("..X.");

	tetromino[1].append("....");
	tetromino[1].append(".XX.");
	tetromino[1].append(".XX.");
	tetromino[1].append("....");
		
	tetromino[2].append("..X.");
	tetromino[2].append("..X.");
	tetromino[2].append(".XX.");
	tetromino[2].append("....");

	tetromino[3].append("..X.");
	tetromino[3].append(".XX.");
	tetromino[3].append("..X.");
	tetromino[3].append("....");

	tetromino[4].append("..X.");
	tetromino[4].append(".XX.");
	tetromino[4].append(".X..");
	tetromino[4].append("....");

	tetromino[5].append(".X..");
	tetromino[5].append(".XX.");
	tetromino[5].append("..X.");
	tetromino[5].append("....");

	tetromino[6].append(".X..");
	tetromino[6].append(".X..");
	tetromino[6].append(".XX.");
	tetromino[6].append("....");


	Field = new char[FieldWidth*FieldHeight]; 
	for (int x = 0; x < FieldWidth; x++)
	{
		for (int y = 0; y < FieldHeight; y++)
		{
			if(x == 0 || x == FieldWidth - 1 || y == FieldHeight - 1)
			{
				Field[y*FieldWidth + x] =  9;
			}
			else
			{
				Field[y*FieldWidth + x] = 0;
			}
		}
	} 

	char *screen = new char[ScreenWidth*ScreenHeight];
	for (int i = 0; i < ScreenWidth*ScreenHeight; i++) screen[i] = ' ';
	HANDLE Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Console);
	DWORD BytesWritten = 0;
	bool GameOver = false;

	srand (time(NULL));
	int CurrentRotation = 0;
	int CurrentPiece = rand()%7;
	int NextPiece = rand()%7;
	int CurrentX = 4;
	int CurrentY = 0;
	bool Key[4];
	bool RotateHold = true;
	vector<int> Lines;
	bool ForceDown = false;
	int Speed = 20;
	int SpeedCount = 0;
	int PieceCount = 0;
	int Score = 0;

	while (!GameOver) 
	{
		this_thread::sleep_for(50ms);
		SpeedCount++;
		ForceDown = (SpeedCount == Speed);

		for (int k = 0; k < 4; k++)
		{
			Key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26"[k]))) != 0;
		}								

		if(Key[0])
		{
			if(DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX + 1, CurrentY))
			{
				CurrentX += 1;
			}
		}
		if(Key[1])
		{
			if(DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX - 1, CurrentY))
			{
				CurrentX -= 1;
			}
		}
		if(Key[2])
		{
			if((DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1)))
			{
				CurrentY += 1;
			}
		}
		if (Key[3])
		{
			if(RotateHold)
			{
				if(DoesPieceFit(CurrentPiece, CurrentRotation + 1, CurrentX, CurrentY))
				{
					CurrentRotation += 1;		
					RotateHold = false;
				}
			}
		}
		else
		{
			RotateHold = true;
		}

		if (ForceDown)
		{
			SpeedCount = 0;
			PieceCount++;
			if (PieceCount % 20 == 0)
				if (Speed >= 10) Speed--;
			
			if (DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1))
				CurrentY++; 
			else
			{
				for (int x = 0; x < 4; x++)
					for (int y = 0; y < 4; y++)
						if (tetromino[CurrentPiece][Rotate(x, y, CurrentRotation)] != '.')
							Field[(CurrentY + y) * FieldWidth + (CurrentX + x)] = CurrentPiece + 1;

				for (int y = 0; y < 4; y++)
					if(CurrentY + y < FieldHeight - 1)
					{
						bool bLine = true;
						for (int x = 1; x < FieldWidth - 1; x++)
							bLine &= (Field[(CurrentY + y) * FieldWidth + x]) != 0;

						if (bLine)
						{
							for (int x = 1; x < FieldWidth - 1; x++)
								Field[(CurrentY + y) * FieldWidth + x] = 8;
							Lines.push_back(CurrentY + y);
						}						
					}

				Score += 25;
				if(!Lines.empty())	Score += (1 << Lines.size()) * 100;

				CurrentX = 4;
				CurrentY = 0;
				CurrentRotation = 0;
				CurrentPiece = NextPiece;
				NextPiece = rand() % 7;


				GameOver = !DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY);
			}
		}


		for (int x = 0; x < FieldWidth; x++)
		{
			for (int y = 0; y < FieldHeight; y++)
			{	
				screen[(y)*ScreenWidth + (x)] = " ABCDEFG*#"[Field[y*FieldWidth + x]];
			}
		}

		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				if (tetromino[CurrentPiece][Rotate(x, y, CurrentRotation)] != '.')
				{
					screen[(CurrentY + y)*ScreenWidth + (CurrentX + x)] = CurrentPiece + 65;
				}
			}
		}

		screen[(2)*ScreenWidth +1+ FieldWidth]='N';
		screen[(2)*ScreenWidth +2+ FieldWidth]='e';
		screen[(2)*ScreenWidth +3+ FieldWidth]='x';
		screen[(2)*ScreenWidth +4+ FieldWidth]='t';
		screen[(2)*ScreenWidth +5+ FieldWidth]=' ';
		screen[(2)*ScreenWidth +6+ FieldWidth]='p';
		screen[(2)*ScreenWidth +7+ FieldWidth]='i';
		screen[(2)*ScreenWidth +8+ FieldWidth]='e';
		screen[(2)*ScreenWidth +9+ FieldWidth]='c';
		screen[(2)*ScreenWidth +10+ FieldWidth]='e';


		for(int x = 0;x < 6;x++)
		{
			for(int y = 0; y < 6;y++)
			{
				if(x == 0 || x == 5 || y == 0 || y == 5)
				{
					screen[(y+3)*ScreenWidth+x+FieldWidth +2 ] = '#';
				}
				else if(tetromino[NextPiece][(y-1)*4+x-1] == 'X')
				{
					screen[(y+3)*ScreenWidth+x+FieldWidth +2 ] = NextPiece + 65;
				}
				else
				{
					screen[(y+3)*ScreenWidth+x+FieldWidth +2 ] = ' ';
				}
			}
		}

		if (!Lines.empty())
		{
			WriteConsoleOutputCharacter(Console, screen, ScreenWidth * ScreenHeight, { 0,0 }, &BytesWritten);
			this_thread::sleep_for(400ms);

			for (auto &v : Lines)
				for (int x = 1; x < FieldWidth - 1; x++)
				{
					for (int py = v; py > 0; py--)
						Field[py * FieldWidth + x] = Field[(py - 1) * FieldWidth + x];
					Field[x] = 0;
				}

			Lines.clear();
		}
		WriteConsoleOutputCharacter(Console, screen, ScreenWidth * ScreenHeight, { 0,0 }, &BytesWritten);
	}
    CloseHandle(Console);
	cout << "Game Over!! Score:" << Score << endl;
	system("pause");
	return 0;
}