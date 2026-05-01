#include <iostream>
#include <time.h>
#include <conio.h>
#include <windows.h>

enum eDir { STOP = 0, LEFT = 1, UPLEFT = 2, DOWNLEFT = 3, RIGHT = 4, UPRIGHT = 5, DOWNRIGHT = 6 }; //all possible movement for the ball
unsigned int illusionTime = 25;

class cBall //stores ball position
{
private:
	int x, y;
	int originalX, originalY;
	eDir direction;

public:
	cBall(int posX, int posY)
	{
		originalX = posX;
		originalY = posY;
		x = posX; y = posY;
		direction = STOP;
	}

	void Reset()
	{
		x = originalX; y = originalY;
		direction = STOP;
	}

	void changeDirection(eDir d)
	{
		direction = d;
	}

	void randomDirection() //random direction ball goes when game starts or scored
	{
		direction = (eDir)((rand() % 6) + 1);
	}

	inline int getX() { return x; }
	inline int getY() { return y; }
	inline eDir getDirection() { return direction; }

	void move()
	{
		switch (direction)
		{
		case STOP: break;
		case LEFT: x--; break;
		case RIGHT: x++; break;
		case UPLEFT: x--; y--; break;
		case DOWNLEFT: x--; y++; break;
		case UPRIGHT: x++; y--; break;
		case DOWNRIGHT: x++; y++; break;
		}
	}
};

class cPaddle //
{
private:
	int x, y;
	int originalX = 0, originalY = 0;

public:
	cPaddle() { x = y = 0; }

	cPaddle(int posX, int posY) : cPaddle()
	{
		originalX = posX;
		originalY = posY;
		x = posX;
		y = posY;
	}

	inline void Reset() { x = originalX; y = originalY; }
	inline int getX() { return x; }
	inline int getY() { return y; }
	inline void moveUp() { y--; }
	inline void moveDown() { y++; }
};

class cGameManager //controls game
{
private:
	int width, height;
	int score1, score2;
	char up1, down1, up2, down2;
	bool quit;
	cBall* ball;
	cPaddle* player1;
	cPaddle* player2;

public:
	cGameManager(int w, int h)
	{
		srand((unsigned int)time(NULL));
		quit = false;
		up1 = 'w'; up2 = 'i';
		down1 = 's'; down2 = 'k';
		score1 = score2 = 0;
		width = w; height = h;

		ball = new cBall(w / 2, h / 2);
		player1 = new cPaddle(1, h / 2 - 3);
		player2 = new cPaddle(w - 2, h / 2 - 3);
	}

	~cGameManager()
	{
		delete ball;
		delete player1;
		delete player2;
	}

	void ScoreUp(cPaddle* player)
	{
		if (player == player1) score1++;
		else if (player == player2) score2++;

		ball->Reset();
		player1->Reset();
		player2->Reset();
	}

	void Draw() //renders game visuals
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		auto setColor = [&](int color)
			{
				SetConsoleTextAttribute(hConsole, color);
			};

		auto resetColor = [&]()
			{
				SetConsoleTextAttribute(hConsole, 7);
			};

		for (int i = 0; i < width + 2; i++)
			std::cout << "\xB2";
		std::cout << std::endl;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int ballx = ball->getX();
				int bally = ball->getY();
				int p1x = player1->getX();
				int p2x = player2->getX();
				int p1y = player1->getY();
				int p2y = player2->getY();

				if (j == 0)
					std::cout << "\xB2";

				// ?? PLAYER 1 (RED)
				if ((p1x == j && p1y == i) ||
					(p1x == j && p1y + 1 == i) ||
					(p1x == j && p1y + 2 == i) ||
					(p1x == j && p1y + 3 == i))
				{
					setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
					std::cout << "\xDB";
					resetColor();
				}

				// ?? PLAYER 2 (BLUE)
				else if ((p2x == j && p2y == i) ||
					(p2x == j && p2y + 1 == i) ||
					(p2x == j && p2y + 2 == i) ||
					(p2x == j && p2y + 3 == i))
				{
					setColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					std::cout << "\xDB";
					resetColor();
				}

				// ? BALL
				else if (ballx == j && bally == i)
				{
					std::cout << "O";
				}

				else
				{
					std::cout << " ";
				}

				if (j == width - 1)
					std::cout << "\xB2";
			}
			std::cout << std::endl;
		}

		for (int i = 0; i < width + 2; i++)
			std::cout << "\xB2";
		std::cout << std::endl;

		std::cout << "Score 1: " << score1
			<< "        Score 2: " << score2 << std::endl;
	}

	void clearScreen()
	{
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD pos = { 0, 0 };
		SetConsoleCursorPosition(hOut, pos);
	}

	void Input() //player controls
	{
		ball->move();

		if (_kbhit())
		{
			char current = _getch();

			if (current == up1 && player1->getY() > 0)
				player1->moveUp();

			if (current == up2 && player2->getY() > 0)
				player2->moveUp();

			if (current == down1 && player1->getY() + 4 < height)
				player1->moveDown();

			if (current == down2 && player2->getY() + 4 < height)
				player2->moveDown();

			if (ball->getDirection() == STOP)
				ball->randomDirection();

			if (current == 'q')
				quit = true;

			illusionTime = 25;
		}
	}

	void Logic() //game physics and collision
	{
		int ballx = ball->getX();
		int bally = ball->getY();

		int p1x = player1->getX();
		int p2x = player2->getX();
		int p1y = player1->getY();
		int p2y = player2->getY();

		for (int i = 0; i < 4; i++)
		{
			if (ballx == p1x + 1 && bally == p1y + i)
				ball->changeDirection((eDir)((rand() % 3) + 4));

			if (ballx == p2x - 1 && bally == p2y + i)
				ball->changeDirection((eDir)((rand() % 3) + 1));
		}

		if (bally == 0)
			ball->changeDirection(ball->getDirection() == UPRIGHT ? DOWNRIGHT : DOWNLEFT);

		if (bally == height - 1)
			ball->changeDirection(ball->getDirection() == DOWNRIGHT ? UPRIGHT : UPLEFT);

		if (ballx == 0) ScoreUp(player2);
		if (ballx == width - 1) ScoreUp(player1);
	}

	void Run() //game loops
	{
		int count = 0;

		while (!quit)
		{
			Draw();
			Input();
			Logic();
			Sleep(illusionTime);
			clearScreen();

			if (illusionTime > 0)
			{
				count++;
				if (count > 25)
				{
					illusionTime--;
					count = 0;
				}
			}
		}
	}
};

int main()
{
	cGameManager game(40, 20);
	game.Run();
	return 0;
}