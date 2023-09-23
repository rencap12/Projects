
if(!window.isOpen() && nameCheck ) {
fstream inFile("files/board_config.cfg", ios_base::in);
string temp = "";
getline(inFile, temp);
int width = stoi(temp);
getline(inFile, temp);
int height = stoi(temp);
getline(inFile, temp);
int numBombs = stoi(temp);
int board[width][height];

//load hidden
for (int i = 0; i < width; i++) {
for (int j = 0; j < height; j++) {
board[i][j] = 10;
}
}
//load mines
for (int k = 0; k < numBombs; k++) {
int bombPw = rand() % width;
int bombPh = rand() % height;
if (board[bombPw][bombPh] != 9) {
board[bombPw][bombPh] = 9;
}
}
Sprite bomb;
Texture bombT;
bombT.loadFromFile("files/images/mine.png");
bomb.setTexture(bombT);

RenderWindow gameWindow(sf::VideoMode((width * 32), (height * 32) + 100), "Game Window", sf::Style::Close);

while (gameWindow.isOpen()) {
Vector2f position = Vector2f(Mouse::getPosition(gameWindow).x / width,
                             Mouse::getPosition(gameWindow).y / height);
bool withinBoard = position.x >= 0 && position.x < width * 32 && position.y >= 0 && position.y < height * 32;
float tile_location_x = position.x;
float tile_location_y = position.y;
float mouse_x;
float mouse_y;
gameWindow.clear(Color::White);

/*
 0 is empty
 1-8 is number
 9 is mine
 10 is unrevealed square
 11 is flag
  */
Event game;

for (int q = 0; q < width; q++) {
for (int r = 0; r < height; r++) {
if(board[q][r] == 200){
Sprite reveal;
Texture revealT;
//change to reveal
revealT.loadFromFile("files/images/tile_revealed.png");
reveal.setTexture(revealT);
reveal.setPosition(floor(mouse_x / 32) * 32, floor(mouse_y / 32) * 32);
gameWindow.draw(reveal);
}
else {
Sprite hidden;
Texture texture;
texture.loadFromFile("files/images/tile_hidden.png");
hidden.setTexture(texture);
hidden.setPosition(q * 32, 32 * r);
gameWindow.draw(hidden);
}
}
}

Sprite happyFace;
Texture happy;
if (!happy.loadFromFile("files/images/face_happy.png")) {
cout << "error loading texture.";
}
happyFace.setTexture(happy);
happyFace.setPosition(((width / 2.0) * 32) - 32, 32 * ((height) + 0.5f));
gameWindow.draw(happyFace);

Sprite debugB;
Texture debugT;
if (!debugT.loadFromFile("files/images/debug.png")) {
cout << "error loading texture.";
}
debugB.setTexture(debugT);
debugB.setPosition(((width) * 32) - 304, 32 * ((height) + 0.5f));
gameWindow.draw(debugB);

Sprite playPause;
Texture playPauseT;
if (!playPauseT.loadFromFile("files/images/pause.png")) {
cout << "error loading texture.";
}
playPause.setTexture(playPauseT);
playPause.setPosition(((width) * 32) - 240, 32 * ((height) + 0.5f));
gameWindow.draw(playPause);
//change to play button when game is paused

Sprite leaderB;
Texture leaderT;
if (!leaderT.loadFromFile("files/images/leaderboard.png")) {
cout << "error loading texture.";
}
leaderB.setTexture(leaderT);
leaderB.setPosition(((width) * 32) - 176, 32 * ((height) + 0.5f));
gameWindow.draw(leaderB);

//FIGURE OUT CLICK AND REVEALING RESPECTIVE TILES
while (gameWindow.pollEvent(game)) {
switch (game.type) {
case ::Event::Closed :
gameWindow.close();
break;

case ::Event::MouseButtonPressed:
if (game.mouseButton.button == Mouse::Left && withinBoard) {
mouse_x = Mouse::getPosition(gameWindow).x;
mouse_y = Mouse::getPosition(gameWindow).y;
board[(int) floor(mouse_x / 32) * 32][(int) floor(mouse_y / 32) * 32] = 200;
}
break;

}

}
gameWindow.display();

}
}

if(clock.getElapsedTime().asSeconds() / 60 > 9) { //left minute image 10 min or more
int minute = floor(elapsedTime_.asSeconds() / 60);
string minuteL = to_string(minute);
char minuteTens = minuteL[0];
timerT2.loadFromFile("files/images/digits.png", IntRect(21 * (int)minuteTens, 32 * (int)minuteTens, 21, 32));
timerM2.setTexture(timerT2);
timerM2.setPosition((columns * 32) - 97, 32 * ((rows + 0.5f) + 16));

char minuteOnes = minuteL[1];
timerT3.loadFromFile("files/images/digits.png", IntRect(21 * (int)minuteOnes, 32 * (int)minuteOnes, 21, 32));
timerM3.setTexture(timerT3);
timerM3.setPosition((columns * 32) - 76, 32 * ((rows + 0.5f) + 16));
}
else {
//left minute image less than 10 min
int minute2 = 0;
timerT2.loadFromFile("files/images/digits.png", IntRect(21 * minute2, 32 * minute2, 21, 32));
timerM2.setTexture(timerT2);
timerM2.setPosition((columns * 32) - 97, 32 * ((rows + 0.5f) + 16));


int minute3 = floor((elapsedTime_.asSeconds()) / 60);
timerT3.loadFromFile("files/images/digits.png", IntRect(21 * minute3, 32 * minute3, 21, 32));
timerM3.setTexture(timerT3);
timerM3.setPosition((columns * 32) - 76, 32 * ((rows + 0.5f) + 16));
}
window.draw(timerM2);
window.draw(timerM3);
//seconds
Sprite timerS2;
Texture timerST2;
Sprite timerS3; //right seconds image
Texture timerST3;
if(elapsedTime_.asSeconds() >= 10) { //left second image up to 5
int seconds = elapsedTime_.asSeconds();
int temp = (seconds % 60) ;
string min = to_string(temp);
char tempBig = min[0];

timerST2.loadFromFile("files/images/digits.png", IntRect(21 * (int)tempBig, 32 * (int)tempBig, 21, 32));
timerS2.setTexture(timerST2);
timerS2.setPosition((columns * 32) - 54, 32 * ((rows + 0.5f) + 16));

char tempSmall = min[1];
timerST3.loadFromFile("files/images/digits.png", IntRect(21 * (int)tempSmall, 32 * (int)tempSmall, 21, 32));
timerS3.setTexture(timerST3);
timerS3.setPosition((columns * 32) - 33, 32 * ((rows + 0.5f) + 16));

}
else {
//left second image less than 10 sec
int seconds2 = 0;
timerST2.loadFromFile("files/images/digits.png", IntRect(21 * seconds2, 32 * seconds2, 21, 32));
timerS2.setTexture(timerST2);
timerS2.setPosition((columns * 32) - 54, 32 * ((rows + 0.5f) + 16));

int seconds3 = (elapsedTime_.asSeconds());
timerST3.loadFromFile("files/images/digits.png", IntRect(21 * seconds3, 32 * seconds3, 21, 32));
timerS3.setTexture(timerST3);
timerS3.setPosition((columns * 32) - 33, 32 * ((rows + 0.5f) + 16));
}

