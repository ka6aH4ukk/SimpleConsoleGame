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
private:
	int x;
	int y;
	char symbol;

public:

	int getX() const { return x; }
	int getY() const { return y; }
	int getSymbol() const { return symbol; }

    void moving(int dx, int dy) {

		if ( x + dx <= 39 && x + dx >= 0 ) {
			x = x + dx;
		}
		if ( y + dy <= 19 && y + dy >= 0 ) {
			y = y + dy;
		}
        
    }
    void enemy_moving(const Entity& target) {
		int diff_x = target.x - x;
		int diff_y = target.y - y;
		int nDiff_x = target.x - x;
		int nDiff_y = target.y - y;
		int i_diff_x = abs(diff_x);
		int i_diff_y = abs(diff_y);
		int i_nDiff_x = abs(nDiff_x);
		int i_nDiff_y = abs(nDiff_y);
		if (i_diff_x > i_diff_y) {
			if (diff_x < 0) {
				if (x != 0) {
					x--;
				}
			}
			else {
				if (x != 39) {
					x++;
				}
			}
		}
		else {
			if (diff_y < 0) {
				if (y != 0) {
					y--;
				}
			}
			else {
				if (y != 19) {
					y++;
				}
			}
		}

    }

	void respawning() {
			x = rand() % 38 +1;
			y = rand() % 18 +1;
	}
	bool collision_check(const Entity& other) const {
		return x == other.x && y == other.y;
	}

	void fleeFrom(const Entity& threat) {
		int diff_x = threat.x - x;
		int diff_y = threat.y - y;
		int step_x = (diff_x > 0) ? -1 : (diff_x < 0 ? 1 : (rand() % 2 == 0 ? 1 : -1));
		int step_y = (diff_y > 0) ? -1 : (diff_y < 0 ? 1 : (rand() % 2 == 0 ? 1 : -1));
		bool can_move_x = (x + step_x >= 0 && x + step_x <= 39);
		bool can_move_y = (y + step_y >= 0 && y + step_y <= 19);
		if (abs(diff_x) >= abs(diff_y) && can_move_x) {
			x += step_x;
		} 
		else if (can_move_y) {
			y += step_y;
		} 
		else if (can_move_x) {
			x += step_x;
    }
}

    Entity(int pos_x, int pos_y, char sym)
    {
        x = pos_x;
        y = pos_y;
		symbol = sym;
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




int main(int argc, char *argv[])
{
	
	for (int j = 0; j < 40; j++) {
		for (int p = 0; p < 20; p++) {
			Main.arr[j][p] = ':';
		}
	}

	srand(time(0));

	matrix();

    Entity player(0, 0,'#');
    Entity enemy(39, 19, '@');
    Entity npc(rand() % 38 + 1, rand() % 18 + 1, '&');
	
	Main.arr[npc.getX()][npc.getY()] = npc.getSymbol();
	Main.arr[enemy.getX()][enemy.getY()] = enemy.getSymbol();
	Main.arr[player.getX()][player.getY()] = player.getSymbol();
	
	matrix();

	while(Main.IsRunning) {
		matrix();
		
		Main.q++;
		Main.arr[enemy.getX()][enemy.getY()] = ':';
		Main.arr[npc.getX()][npc.getY()] = ':';
		if (Main.q%2 == 0) {
			npc.fleeFrom(player);
			enemy.enemy_moving(player);
		}

		Main.arr[player.getX()][player.getY()] = ':';

		Main.choice = gettingChar();

		switch(Main.choice) {
			case 'a': player.moving(-1, 0);
				break;
			case 'd': player.moving(1, 0);
				break;
			case 'w': player.moving(0, -1);
				break;
			case 's': player.moving(0, 1);
				break;
			case 'q': Main.IsRunning = 0;
		}
		
		Main.arr[npc.getX()][npc.getY()] = '&';
		Main.arr[enemy.getX()][enemy.getY()] = '@';
		Main.arr[player.getX()][player.getY()] = '#';
		
		if (player.collision_check(enemy)) {
			Main.IsRunning = false;
			cout << "Game Over!" << endl;
			cout << "Your score: " << Main.score << endl;
			cout << "Your lifetime: " << Main.LifeTime << endl;
		}
		if (player.collision_check(npc)) {
			npc.respawning();
			Main.score++;
		}
		Main.LifeTime++;
	}
}
