#include <iostream>
#include <ctime>
#include <termios.h>
#include <unistd.h>
#include <cstdlib>
#include <cmath>


using namespace std;
struct Main {
	char arr[40][20];
	bool IsRunning = true;
	char choice = ' ';
	int q = 0;
	int score = 0;
	int LifeTime = 0;
} Main;

class Entity
{
public:
    int x;
    int y;

    int moving(int wll, int dirctn, int& axis) {
        if (axis != wll) {
		if (dirctn) {
			axis++;
            }
		else {
			axis--;
            }
        }
        return axis;
    }
    void npc_moving(int wll, int &mov, int diff) {
		if (diff < 0) {
			if (mov != 0) {
				mov--;
			}
		}
		else {
			if (mov != wll) {
				mov++;
			}
		}
    }

    Entity(int pos_x, int pos_y)
    {
        x = pos_x;
        y = pos_y;
    }

    


};	

void matrix() {
	cout << "\x1B[2J\x1B[H";
	for (int i = 0; i < 20; i++) {
		for (int k = 0;k < 40; k++) {
			cout<<Main.arr[k][i];
		}
		cout<<endl;
	}
}

void collision_check(bool &IsR, int LiT, Entity& player, Entity& enemy, Entity& npc) {
	if (player.x == npc.x && player.y == npc.y) {
		Main.score++;
		npc.x = rand() % 38 + 1;
		npc.y = rand() % 18 + 1;
	}

	if (player.x == enemy.x && player.y == enemy.y) {
			matrix();
            cout<<"Game over!\nYour life time: "<<LiT<<endl;
			cout<<"Your score: "<<Main.score<<endl;
            IsR = false;
	}
}

void setRawMode() {
	termios oldSettings;
	tcgetattr(STDIN_FILENO, &oldSettings);
	termios newSettings = oldSettings;
	newSettings.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &newSettings);
}

void restoreTerminal() {
	termios defaultSettings;
	tcgetattr(STDIN_FILENO, &defaultSettings);
	defaultSettings.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &defaultSettings);
}

char gettingChar() {
	setRawMode();
	char ch;
	bool isGetting = 1;
	while (isGetting) {
		if (read(STDIN_FILENO, &ch, 1 ) > 0) {
			isGetting = 0;
		}
	}
	restoreTerminal();
	return ch;
}

void update_npc(Entity& npc, const Entity& player) {
    int diff_x = player.x - npc.x;
    int diff_y = player.y - npc.y;
    int step_x = (diff_x > 0) ? -1 : (diff_x < 0 ? 1 : (rand() % 2 == 0 ? 1 : -1));
    int step_y = (diff_y > 0) ? -1 : (diff_y < 0 ? 1 : (rand() % 2 == 0 ? 1 : -1));
    bool can_move_x = (npc.x + step_x >= 0 && npc.x + step_x <= 39);
    bool can_move_y = (npc.y + step_y >= 0 && npc.y + step_y <= 19);
    if (abs(diff_x) >= abs(diff_y) && can_move_x) {
        npc.x += step_x;
    } 
    else if (can_move_y) {
        npc.y += step_y;
    } 
    else if (can_move_x) {
        npc.x += step_x;
    }
}


int main(int argc, char *argv[])
{
	
	for (int j = 0; j < 40; j++) {
		for (int p = 0; p < 20; p++) {
			Main.arr[j][p] = ':';
		}
	}

	srand(time(0));

	matrix();

    Entity player(0, 0);
    Entity enemy(39, 19);
    Entity npc(rand() % 38 + 1, rand() % 18 + 1);
	
	Main.arr[npc.x][npc.y] = '&';
	Main.arr[enemy.x][enemy.y] = '@';
	Main.arr[player.x][player.y] = '#';
	
	matrix();

	while(Main.IsRunning) {
		matrix();
		Main.arr[enemy.x][enemy.y] = ':';
		Main.arr[npc.x][npc.y] = ':';
		
		Main.q++;

		int diff_x = player.x - enemy.x;
		int diff_y = player.y - enemy.y;
		int nDiff_x = player.x - npc.x;
		int nDiff_y = player.y - npc.y;
		int i_diff_x = abs(diff_x);
		int i_diff_y = abs(diff_y);
		int i_nDiff_x = abs(nDiff_x);
		int i_nDiff_y = abs(nDiff_y);
		if (Main.q%2 == 0) {
//			if (i_nDiff_x < i_nDiff_y) {
//				npc.npc_moving(19, npc.y, nDiff_y*-1);
//			}
//			else {
//				npc.npc_moving(39, npc.x, nDiff_x*-1);
//			}

			update_npc(npc, player);

			if (i_diff_x < i_diff_y) {
				npc.npc_moving(19, enemy.y, diff_y);
			}
			else {
				npc.npc_moving(39, enemy.x, diff_x);
			}
		}

		Main.arr[player.x][player.y] = ':';

		Main.choice = gettingChar();

		switch(Main.choice) {
			case 'a': player.moving(0, 0, player.x);
				break;
			case 'd': player.moving(39, 1, player.x);
				break;
			case 'w': player.moving(0, 0, player.y);
				break;
			case 's': player.moving(19, 1, player.y);
				break;
			case 'q': Main.IsRunning = 0;
		}
		
		Main.arr[npc.x][npc.y] = '&';
		Main.arr[enemy.x][enemy.y] = '@';
		Main.arr[player.x][player.y] = '#';
		
		collision_check(Main.IsRunning, Main.LifeTime, player, enemy, npc);
		Main.LifeTime++;
	}
}
