/* This game was made by closely following YouTuber FamTrinli's video "Let's make 16
*  games in C++: TETRIS" 
*  (url: https://www.youtube.com/watch?v=zH_omFPqMO4&list=WL&index=131 ) for the
*  purposes of practice and education; this version of the game was programmed using
*  the procedural style of code creation, and slight modifications have been made to
*  the version of the game made in the video.
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <string>
using namespace sf;

//20 rows, 12 columns
const int M = 20;
const int N = 12;

//An array defining the "field" that the minoes are allowed to travel in
int field[M][N] = {0};

//A struct defining what a point is
struct Point {
	int x = 6;
	int y = 32;
}	a[4], b[4];
//The "points" of where each piece of the minoes are (a) and where they will be 
//relocated to if they try to leave the field (b).

//An array to store each of the tetrominoes
int tetros[7][4] = {
	{ 1,3,5,7 }, //I
	{ 0,2,3,5 }, //Z
	{ 1,3,2,4 }, //S
	{ 1,3,2,5 }, //T
	{ 0,1,3,5 }, //L
	{ 1,3,5,4 }, //J
	{ 0,1,2,3 }  //O
};

//Bounds checking (the minoes cannot move outside a certain area)
bool check() {
	for (int i=0;i<4;++i) {
		if (a[i].x<0 || a[i].x>=N || a[i].y>=M)
			return false;
		else if (field[a[i].y][a[i].x])
			return false;
	}

	return true;
};

int main() {
	//The random seed
	srand(time(0));

	//make the window the game is in
	RenderWindow window(VideoMode(320, 480), "Tetris - Procedural");

	//The textures we'll be using
	Texture t1, t2, t3, t4;
	t1.loadFromFile("../../assets/Tetris/sprites/tiles.png");
	t2.loadFromFile("../../assets/Tetris/sprites/background.png");
	t3.loadFromFile("../../assets/Tetris/sprites/PlayButton.png");

	//Make sprites out of the textures
	Sprite s(t1), background(t2), playbut(t3);
	s.setTextureRect(IntRect(0,0,18,18));
	playbut.setPosition(139, 415);
	playbut.setColor(Color(255, 255, 255, 0));

	//The font and text for the score
	Font scoreFont;
	scoreFont.loadFromFile("../../assets/Tetris/fonts/arial.ttf");
	Text scoreText;
	scoreText.setFont(scoreFont);
	scoreText.setCharacterSize(24);
	scoreText.setFillColor(Color::Red);
	scoreText.setPosition(154, 1);
	Text GameOverText;
	GameOverText.setFont(scoreFont);
	GameOverText.setCharacterSize(50);
	GameOverText.setFillColor(Color::Red);
	GameOverText.setPosition(91, 86);
	GameOverText.setString("Game\nOver!");

	//The sounds and music we'll be using
	SoundBuffer sb1;
	sb1.loadFromFile("../../assets/Tetris/sounds/sfx/shimmer.flac");
	Music bgmusic;
	bgmusic.openFromFile("../../assets/Tetris/sounds/music/Arcade.ogg");
	bgmusic.play();

	//Make sounds out of those buffers
	Sound shimmer;
	shimmer.setBuffer(sb1);

	//Variables
	int dx = 0; //Move direction (-1 = left, 1 = right)
	int score = 0; //The player's score
	int colorNum = 1; //which color the next mino will be
	float timer = 0, delay = .4; //Drop every .4 seconds
	bool rotate = false; //Are we rotating this frame?
	bool pause = false; //Are we paused?
	bool GameOver = false; //Have we lost?
	scoreText.setString(std::to_string(score));

	//A clock for determining when our next move is
	Clock clock;

	//While the game is playing
	while (window.isOpen()) {
		//How many seconds have elapsed [since last frame?]
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer+=time;

		//Event polling
		Event e;
		while (window.pollEvent(e)) {
			//if the player closes the window, stop the music and close the window
			if (e.type == Event::Closed) {
				bgmusic.stop();
				window.close();
			}

			if (!GameOver) {
				//Set our variables according to the input we've received this frame
				//(works when key is pressed and held, but only in accordance with OS delay)
				if (e.type == Event::KeyPressed) {
					if (e.key.code == Keyboard::Up)
						rotate = true;
					else if (e.key.code == Keyboard::Left)
						dx = -1;
					else if (e.key.code == Keyboard::Right)
						dx = 1;
				}

				//Pause
				if (e.type == Event::MouseButtonReleased) {
					Vector2i mousePos = Mouse::getPosition(window);
					if ((mousePos.x > 139 && mousePos.x < 179) &&
						(mousePos.y > 415 && mousePos.y < 453)) {

						pause = !pause;

						if (pause)
							playbut.setColor(Color(255, 255, 255, 255));
						else
							playbut.setColor(Color(255, 255, 255, 0));
					}
				}
			}


			///////Move///////
			if (!pause && !GameOver) {
				//Drop
				if (Keyboard::isKeyPressed(Keyboard::Down))
					delay = .05;

				//Move each segment of the mino according to whether we are moving
				//left or right
				for (int i=0;i<4;++i) {
					b[i] = a[i];
					a[i].x += dx;
				}
				//If the mino is about to exit the field
				if (!check()) {
					for (int i=0;i<4;++i)
						a[i] = b[i];
				}
			}


			///////Rotate///////
			//If we are rotating this frame, rotate (clockwise) around the center of 
			//rotation
			if (rotate && !pause && !GameOver) {
				Point p = a[1]; //"center of rotation"
				//Individually rotates each piece of the mino to were it should be
				for (int i=0;i<4;++i) {
					int x = a[i].y-p.y;
					int y = a[i].x-p.x;
					a[i].x = p.x - x;
					a[i].y = p.y + y;
				}
				//If the mino is about to exit the field, don't
				if (!check()) {
					for (int i=0;i<4;++i)
						a[i] = b[i];
				}
			}


			///////Drop tick///////
			//(drops every 1/2 sec)
			if (timer>delay && !pause && !GameOver) {
				//Drop by 1 square (18 pixels)
				for (int i=0;i<4;++i) {
					b[i] = a[i];
					a[i].y += 1;
				}

				//If the mino is not out of bounds
				if (!check()) {
					//Set where the mino should be to 1
					for (int i=0;i<4;++i)
						field[b[i].y][b[i].x] = colorNum;

					//pick one of 8 colors randomly
					colorNum = 1 + rand()%7;

					//Randomly decide which mino to drop next
					int n = rand()%7;
					for (int i=0;i<4;++i) {
						a[i].x = (tetros[n][i]%2)*6;
						if (a[i].x == 0)
							a[i].x = 5;
						a[i].y = tetros[n][i]/2;
					}
				}
				timer=0;
			}


			///////Line Clearing///////
			if (!GameOver) {
				int k=M-1;
				for (int i=M-1;i>0;--i) {
					int count = 0;

					for (int j=0;j<N;++j) {
						if (field[i][j])
							++count;
						field[k][j] = field[i][j];
					}

					if (i == 1) {
						if (count > 2) {
							GameOver = true;
						}
					}

					if (count < N)
						--k;
					else {
						shimmer.play();
						++score;
						scoreText.setString(std::to_string(score));
					}
				}
			}


			/////////GO Checking/////////
			if (!GameOver) {
				int count = 0;
				for (int i = M-1; i>0; --i) {
					for (int j = 0; j < N; ++j) {
						if (field[i][j]) {
							++count;
							break;
						}
					}

					if (count >= M-1)
						GameOver = true;
				}
			}

			//Reset the move, rotate, and delay variables
			dx = 0; rotate = false;	
			if (!pause)
				delay = .4;

			///////Draw///////
			//Clear the stuff from the old frame and draw the new stuff
			window.clear(Color::White);
			window.draw(background);
			window.draw(playbut);
			window.draw(scoreText);

			for (int i=0;i<M;++i) {
				for (int j=0;j<N;++j) {
					if (field[i][j] == 0)
						continue;
					s.setTextureRect(IntRect(field[i][j]*18,0,18,18));
					s.setPosition(j*18, i*18);
					s.move(53, 32); //"offset"
					window.draw(s);
				}
			}
		
			for (int i=0;i<4;++i) {
				s.setTextureRect(IntRect(colorNum*18,0,18,18));
				s.setPosition(a[i].x*18, a[i].y*18);
				s.move(53, 32); //"offset"
				window.draw(s);
			}

			if (GameOver)
				window.draw(GameOverText);

			window.display();
		}
	}

	return 0;
}