#include <SFML/Graphics.hpp>
#include "TextureManager.h"
#include "TextureManager.cpp"
#include "Tile.cpp"
#include "Random.cpp"
#include "Tile.h"
#include "Random.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <SFML/Window.hpp>
#include <exception>
#include <fstream>
#include <cmath>
#include <chrono>

using namespace std;
using namespace sf;


void setText(Text &text, float x, float y){
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
                   textRect.top + textRect.height/2.0f);
    text.setPosition(Vector2f(x, y));
}

void RevealTiles(vector<Tile*> neighbors, Tile* tile) {
    if (tile == NULL) {
        return;
    }
    if (tile->getMineStatus()){
        return;
    }
    if (!tile->getHiddenStatus()) {
        return;
    }
    if (tile->getFlagStatus()) {
        return;
    }
    tile->setHiddenStatus(false);
    if (tile->getMineCount() > 0) {
        return;
    }
    for (int i = 0; i < 8; i++) {
        if (neighbors[i] == NULL) {
            continue;
        }
        RevealTiles(neighbors[i]->GetNeighbors(), neighbors[i]);
    }
}

int main()
{

    srand(time(0));
    RenderWindow windowW(VideoMode(800, 600), "Minesweeper", Style::Close);
    Font font;
    if(!font.loadFromFile("files/font.ttf" )){
        windowW.close();
    }

    Text text;
    text.setFont(font);
    text.setString("WELCOME TO MINESWEEPER!");
    text.setCharacterSize(24);
    text.setStyle(Text::Bold | Text:: Underlined);
    text.setFillColor(Color::White);
    setText(text,(800/2.0f),(600/2.0f - 150));
    Text inputPrompt;
    inputPrompt.setFont(font);
    inputPrompt.setString("Enter your name:");
    inputPrompt.setCharacterSize(20);
    inputPrompt.setStyle(Text::Bold);
    inputPrompt.setFillColor(Color::White);
    setText(inputPrompt,(800/2.0f),(600/2.0f - 75));
    Text name;
    name.setFont(font);
    name.setCharacterSize(18);
    name.setFillColor(Color::Yellow);
    name.setStyle(Text::Bold);
    setText(name, (800/2.0f), (600/2.0f - 45));
    String playerInput;
    Text cursor;
    cursor.setFont(font);
    cursor.setString("|");
    cursor.setCharacterSize(18);
    cursor.setFillColor(Color::Yellow);
    cursor.setStyle(Text::Bold);
    setText(cursor, (800 / 2.0f), (600 / 2.0f - 45));


    bool nameCheck = false;
    while (windowW.isOpen()) {

        // check all the window's events that were triggered since the last iteration of the loop
        Event event;

        windowW.clear(Color::Blue);
        windowW.draw(text);
        windowW.draw(inputPrompt);

        if (name.getString().getSize() == 0) {
            name.setString("|");
            setText(name, (800 / 2.0f), (600 / 2.0f - 45));
            windowW.draw(cursor);
        }

        while (windowW.pollEvent(event)) {
            int counter = 0;
            switch (event.type) {
                case Event::Closed:
                    windowW.close();
                    break;

                case Event::TextEntered:
                    if (name.getString().getSize() <= 10) { //no more than 10 characters for name
                        if ((event.text.unicode >= 65 && event.text.unicode <= 90) ||
                            (event.text.unicode >= 97 && event.text.unicode <= 122)) {
                            //only letters
                            counter++;
                            playerInput += event.text.unicode;
                            name.setString(playerInput + "|");
                            string temp = name.getString();
                            char firstL = (char) toupper(temp[0]);
                            temp[0] = firstL;
                            for (int i = 1; i < name.getString().getSize(); i++) {
                                string lowerL = name.getString();
                                char lowercase = (char) ::tolower(temp[i]);
                                temp[i] = lowercase;
                            }
                            temp = temp.substr(0,temp.length() - 1);
                            name.setString(temp);
                            setText(name, (800 / 2.0f) - counter, (600 / 2.0f - 45));
                            break;
                        }
                    }

                    try {
                        if (event.text.unicode == '\b' && name.getString().getSize() != 0) {
                            counter--;
                            playerInput.erase(playerInput.getSize() - 1, 1);
                            name.setString(playerInput + "|");
                            string temp = name.getString();
                            char firstL = (char) toupper(temp[0]);
                            temp[0] = firstL;
                            for (int i = 1; i < name.getString().getSize(); i++) {
                                string lowerL = name.getString();
                                char lowercase = (char) ::tolower(temp[i]);
                                temp[i] = lowercase;
                            }
                            temp = temp.substr(0,temp.length() - 1);
                            name.setString(temp);
                            setText(name, (800 / 2.0f) + counter, (600 / 2.0f - 45));
                            break;
                        }
                    }
                    catch (exception &e) {}

                case Event::KeyPressed:
                    if (event.key.code == Keyboard::Enter) {
                        //reformat name , close welcome, open game
                        if (name.getString() == "|") {
                            break;
                        }
                        nameCheck = true;
                        windowW.close(); //close welcome
                        break;
                    }

            }
        }
        windowW.draw(name);
        windowW.display();
    }

    ifstream file("files/board_config.cfg");
    string columnsS, rowsS, minesS;
    if (file.is_open()) {
        getline(file, columnsS, '\n');
        getline(file, rowsS, '\n');
        getline(file, minesS, '\n');
    }
    int columns, rows, mines;
    columns = stoi(columnsS);
    rows = stoi(rowsS);
    mines = stoi(minesS);
    int ogMineCount = mines;

    Tile tiles[columns][rows];

    bool gameOver = false;
    bool debugMode = false;
    bool winGame = false;
    int counter = mines;

    RenderWindow window(VideoMode(columns * 32, rows * 32 + 100), "Minesweeper");

    int mineCount = 0;
    for (int i = 0; mineCount < mines; i++) {
        int mineCol = Random::Int(0, columns - 1);
        int mineRow = Random::Int(0, rows - 1);
        if (tiles[mineCol][mineRow].getMineStatus()){
            continue;
        }
        else {
            tiles[mineCol][mineRow].setMineStatus(true);
            mineCount++;
        }
    }

    Sprite tileSprites[columns][rows];

    for (int i = 0; i < rows; i++) {    //sets up all the tiles
        for (int j = 0; j < columns; j++) {
            Sprite newSprite(tiles[j][i].getSprite());
            newSprite.setPosition(32 * j, 32 * i);
            tileSprites[j][i] = newSprite;

            if (i == 0 && j == 0) {  //top left
                tiles[j][i].SetNeighbors(nullptr, nullptr, nullptr, nullptr, &tiles[j+1][i], nullptr, &tiles[j][i+1], &tiles[j+1][i+1]);
            }
            else if (i == 0 && j == columns - 1) {  //top right
                tiles[j][i].SetNeighbors(nullptr, nullptr, nullptr, &tiles[j-1][i], nullptr, &tiles[j-1][i+1], &tiles[j][i+1], nullptr);
            }
            else if (j == 0 && i == rows - 1) {   //bottom left
                tiles[j][i].SetNeighbors(nullptr, &tiles[j][i-1], &tiles[j+1][i-1], nullptr, &tiles[j+1][i], nullptr, nullptr, nullptr);
            }
            else if (j == columns - 1 && i == rows - 1) {   //bottom right
                tiles[j][i].SetNeighbors(&tiles[j-1][i-1], &tiles[j][i-1], nullptr, &tiles[j-1][i], nullptr, nullptr, nullptr, nullptr);
            }
            else if (j == 0) {   //left column
                tiles[j][i].SetNeighbors(nullptr, &tiles[j][i-1], &tiles[j+1][i-1], nullptr, &tiles[j+1][i], nullptr, &tiles[j][i+1], &tiles[j+1][i+1]);
            }
            else if (i == 0) {  //top row
                tiles[j][i].SetNeighbors(nullptr, nullptr, nullptr, &tiles[j-1][i], &tiles[j+1][i], &tiles[j-1][i+1], &tiles[j][i+1], &tiles[j+1][i+1]);
            }
            else if (j == columns - 1) {    //right column
                tiles[j][i].SetNeighbors(&tiles[j-1][i-1], &tiles[j][i-1], nullptr, &tiles[j-1][i], nullptr, &tiles[j-1][i+1], &tiles[j][i+1], nullptr);
            }
            else if (i == rows - 1) {   //bottom row
                tiles[j][i].SetNeighbors(&tiles[j-1][i-1], &tiles[j][i-1], &tiles[j+1][i-1], &tiles[j-1][i], &tiles[j+1][i], nullptr, nullptr, nullptr);
            }
            else {  //not outer ring
                tiles[j][i].SetNeighbors(&tiles[j-1][i-1], &tiles[j][i-1], &tiles[j+1][i-1], &tiles[j-1][i], &tiles[j+1][i], &tiles[j-1][i+1], &tiles[j][i+1], &tiles[j+1][i+1]);
            }
        }
    }

    Texture negativeCounterTexture;
    negativeCounterTexture.loadFromFile("files/images/digits.png", IntRect(210,0,21,32));
    Sprite negativeCounterSprite;
    negativeCounterSprite.setTexture(negativeCounterTexture);
    negativeCounterSprite.setPosition(12, 32 * ((rows) + 0.5f) + 16);

    Texture counterTextures[10];

    Texture counter1Texture;
    counter1Texture.loadFromFile("files/images/digits.png", IntRect(21,0,21,32));
    counterTextures[1] = counter1Texture;

    Texture counter2Texture;
    counter2Texture.loadFromFile("files/images/digits.png", IntRect(21 * 2,0,21,32));
    counterTextures[2] = counter2Texture;

    Texture counter3Texture;
    counter3Texture.loadFromFile("files/images/digits.png", IntRect(21 * 3,0,21,32));
    counterTextures[3] = counter3Texture;

    Texture counter4Texture;
    counter4Texture.loadFromFile("files/images/digits.png", IntRect(21 * 4,0,21,32));
    counterTextures[4] = counter4Texture;

    Texture counter5Texture;
    counter5Texture.loadFromFile("files/images/digits.png", IntRect(21 * 5,0,21,32));
    counterTextures[5] = counter5Texture;

    Texture counter6Texture;
    counter6Texture.loadFromFile("files/images/digits.png", IntRect(21 * 6,0,21,32));
    counterTextures[6] = counter6Texture;

    Texture counter7Texture;
    counter7Texture.loadFromFile("files/images/digits.png", IntRect(21 * 7,0,21,32));
    counterTextures[7] = counter7Texture;

    Texture counter8Texture;
    counter8Texture.loadFromFile("files/images/digits.png", IntRect(21 * 8,0,21,32));
    counterTextures[8] = counter8Texture;

    Texture counter9Texture;
    counter9Texture.loadFromFile("files/images/digits.png", IntRect(21 * 9,0,21,32));
    counterTextures[9] = counter9Texture;

    Texture counter0Texture;
    counter0Texture.loadFromFile("files/images/digits.png", IntRect(0,0,21,32));
    counterTextures[0] = counter0Texture;

    Sprite firstCounterSprite;
    firstCounterSprite.setTexture(counter0Texture);
    firstCounterSprite.setPosition(33, 32 * ((rows) + 0.5f) + 16);

    Sprite secondCounterSprite;
    secondCounterSprite.setTexture(counter0Texture);
    secondCounterSprite.setPosition(54, 32 * ((rows) + 0.5f) + 16);

    Sprite thirdCounterSprite;
    thirdCounterSprite.setTexture(counter0Texture);
    thirdCounterSprite.setPosition(75, 32 * ((rows) + 0.5f) + 16);


    Sprite oneSprite;
    oneSprite.setTexture(TextureManager::GetTexture("number_1"));

    Sprite twoSprite;
    twoSprite.setTexture(TextureManager::GetTexture("number_2"));

    Sprite threeSprite;
    threeSprite.setTexture(TextureManager::GetTexture("number_3"));

    Sprite fourSprite;
    fourSprite.setTexture(TextureManager::GetTexture("number_4"));

    Sprite fiveSprite;
    fiveSprite.setTexture(TextureManager::GetTexture("number_5"));

    Sprite sixSprite;
    sixSprite.setTexture(TextureManager::GetTexture("number_6"));

    Sprite sevenSprite;
    sevenSprite.setTexture(TextureManager::GetTexture("number_7"));

    Sprite eightSprite;
    eightSprite.setTexture(TextureManager::GetTexture("number_8"));


    Texture debugTexture;   //debug button
    debugTexture.loadFromFile("files/images/debug.png");
    Sprite debugSprite;
    debugSprite.setTexture(debugTexture);
    debugSprite.setPosition(columns * 32 - 304 , 32 * (rows + 0.5f));



    Texture happyTexture;
    happyTexture.loadFromFile("files/images/face_happy.png");
    Texture loseTexture;
    loseTexture.loadFromFile("files/images/face_lose.png");
    Texture winTexture;
    winTexture.loadFromFile("files/images/face_win.png");
    Sprite winSprite;
    winSprite.setPosition((columns / 2.0) * 32 - 32, 32 * (rows + 0.5f));
    Texture pause;
    pause.loadFromFile("files/images/pause.png");
    Sprite pauseBut;
    pauseBut.setTexture(pause);
    pauseBut.setPosition((columns * 32) - 240, 32 *((rows) + 0.5f));
    Texture play;
    play.loadFromFile("files/images/play.png");
    Texture leaderB;
    leaderB.loadFromFile("files/images/leaderboard.png");
    Sprite leaderBut;
    leaderBut.setTexture(leaderB);
    leaderBut.setPosition((columns * 32) - 176, 32 * (rows + 0.5f));

    Texture timerTextures[10];

    sf::Texture timer1Texture;
    timer1Texture.loadFromFile("files/images/digits.png", sf::IntRect(21,0,21,32));
    timerTextures[1] = timer1Texture;

    sf::Texture timer2Texture;
    timer2Texture.loadFromFile("files/images/digits.png", sf::IntRect(21 * 2,0,21,32));
    timerTextures[2] = timer2Texture;

    sf::Texture timer3Texture;
    timer3Texture.loadFromFile("files/images/digits.png", sf::IntRect(21 * 3,0,21,32));
    timerTextures[3] = timer3Texture;

    sf::Texture timer4Texture;
    timer4Texture.loadFromFile("files/images/digits.png", sf::IntRect(21 * 4,0,21,32));
    timerTextures[4] = timer4Texture;

    sf::Texture timer5Texture;
    timer5Texture.loadFromFile("files/images/digits.png", sf::IntRect(21 * 5,0,21,32));
    timerTextures[5] = timer5Texture;

    sf::Texture timer6Texture;
    timer6Texture.loadFromFile("files/images/digits.png", sf::IntRect(21 * 6,0,21,32));
    timerTextures[6] = timer6Texture;

    sf::Texture timer7Texture;
    timer7Texture.loadFromFile("files/images/digits.png", sf::IntRect(21 * 7,0,21,32));
    timerTextures[7] = timer7Texture;

    sf::Texture timer8Texture;
    timer8Texture.loadFromFile("files/images/digits.png", sf::IntRect(21 * 8,0,21,32));
    timerTextures[8] = timer8Texture;

    sf::Texture timer9Texture;
    timer9Texture.loadFromFile("files/images/digits.png", sf::IntRect(21 * 9,0,21,32));
    timerTextures[9] = timer9Texture;

    sf::Texture timer0Texture;
    timer0Texture.loadFromFile("files/images/digits.png", sf::IntRect(0,0,21,32));
    timerTextures[0] = timer0Texture;

    sf::Sprite firstTimerSprite;
    firstTimerSprite.setTexture(timer0Texture);
    firstTimerSprite.setPosition((columns * 32) - 97, 32 * ((rows) + 0.5f) + 16);

    sf::Sprite secondTimerSprite;
    secondTimerSprite.setTexture(timer0Texture);
    secondTimerSprite.setPosition((columns * 32) - 76, 32 * ((rows) + 0.5f) + 16);

    sf::Sprite thirdTimerSprite;
    thirdTimerSprite.setTexture(timer0Texture);
    thirdTimerSprite.setPosition((columns * 32) - 54, 32 * ((rows) + 0.5f) + 16);

    Sprite fourthTimerSprite;
    fourthTimerSprite.setTexture(timer0Texture);
    fourthTimerSprite.setPosition((columns * 32) - 33, 32 * ((rows) + 0.5f) + 16);


    auto start = std::chrono::high_resolution_clock::now();
    Font fontT;
    fontT.loadFromFile("files/font.ttf");
    Text test("", fontT);
    test.setCharacterSize(30);
    test.setFillColor(Color::Green);
    setText(test,columns / 2.0f, rows / 2.0f );
    bool paused = false;
    Clock clock;
    Time timeRemember = clock.getElapsedTime();

    while (window.isOpen()) {

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePosition = Mouse::getPosition(window);

                if (event.mouseButton.button == Mouse::Left) {

                    for (int i = 0; i < rows; i++) {    //sets up all the tiles and draws them on board
                        for (int j = 0; j < columns; j++) {
                            if (tileSprites[j][i].getGlobalBounds().contains(mousePosition.x, mousePosition.y) &&
                                gameOver == false && winGame == false) {  //clicks on a tile
                                if (tiles[j][i].getFlagStatus() == false) {
                                    if (tiles[j][i].getMineCount() == 0 && !tiles[j][i].getMineStatus() &&
                                        tiles[j][i].getHiddenStatus()) {
                                        RevealTiles(tiles[j][i].GetNeighbors(), &tiles[j][i]);
                                    }
                                    tiles[j][i].setHiddenStatus(false); //click on a tile and reveal it
                                    if (tiles[j][i].getMineStatus()) {
                                        gameOver = true;
                                    }
                                }
                            }
                        }
                    }

                    int revealedTiles = 0;
                    for (int i = 0; i < rows; i++) {
                        for (int j = 0; j < columns; j++) {
                            if (tiles[j][i].getHiddenStatus() == false) {
                                revealedTiles++;
                            }
                            if (revealedTiles == ((columns * rows) - mines)) {
                                winGame = true;
                            }
                        }
                    }

                    if (winGame) {
                        for (int i = 0; i < rows; i++) {
                            for (int j = 0; j < columns; j++) {
                                if (tiles[j][i].getMineStatus()) {
                                    tiles[j][i].setFlagStatus(true);
                                }
                            }
                        }
                        counter = 0;
                    }

                    if (winSprite.getGlobalBounds().contains(mousePosition.x,
                                                             mousePosition.y)) {   //click on smiley face
                        gameOver = false;
                        winGame = false;
                        for (int i = 0; i < rows; i++) {    //makes all tiles have no mines and hidden again first
                            for (int j = 0; j < columns; j++) {
                                tiles[j][i].setMineStatus(false);
                                tiles[j][i].setHiddenStatus(true);
                                tiles[j][i].setFlagStatus(false);
                            }
                        }

                        mines = ogMineCount;
                        mineCount = 0;
                        for (int i = 0; mineCount < mines; i++) {  //then randomizes mines
                            int mineCol = Random::Int(0, columns);
                            int mineRow = Random::Int(0, rows);
                            if (tiles[mineCol][mineRow].getMineStatus()) {
                                continue;
                            } else {
                                tiles[mineCol][mineRow].setMineStatus(true);
                                mineCount++;
                            }
                        }
                        counter = mines;
                    }

                    if (debugSprite.getGlobalBounds().contains(mousePosition.x,
                                                               mousePosition.y)) { //clicks on debug button
                        debugMode = !debugMode;
                    }

                    if (pauseBut.getGlobalBounds().contains(mousePosition.x,
                                                            mousePosition.y)) {
                        paused = !paused;
                        if (paused) {
                            pauseBut.setTexture(play);
                            window.draw(pauseBut);
                            auto pause_time = clock.getElapsedTime();
                            while(paused) {
                                Event pause_event;
                                while (window.pollEvent(pause_event)) {
                                    if (pause_event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left && pauseBut.getGlobalBounds().contains(mousePosition.x,mousePosition.y)){
                                        paused = false;
                                        clock.restart();
                                    }
                                    if(pause_event.type == Event::Closed){
                                        window.close();
                                    }
                                }
                            }
                            auto elapsed = clock.getElapsedTime() - pause_time;
                            timeRemember = elapsed;
                        } else {
                            pauseBut.setTexture(pause);
                            window.draw(pauseBut);
                            timeRemember = clock.getElapsedTime();
                        }
                    }

                } else if (event.mouseButton.button == Mouse::Right && gameOver == false && winGame == false) {
                    for (int i = 0; i < rows; i++) {    //sets up all the tiles and draws them on board
                        for (int j = 0; j < columns; j++) {
                            if (tiles[j][i].getHiddenStatus() ==
                                false) {   //if you right click dont bother looking at tiles that are revealed since they cant have flags
                                continue;
                            }
                            if (tileSprites[j][i].getGlobalBounds().contains(mousePosition.x,
                                                                             mousePosition.y)) {   //right click on hidden tile places or removes flag
                                if (tiles[j][i].getFlagStatus() == false) {
                                    counter--;  //if successful flag placement 1 down on mine counter
                                } else {
                                    counter++;  //if successful flag removal 1 up on mine counter
                                }
                                tiles[j][i].setFlagStatus(!tiles[j][i].getFlagStatus());
                            }
                        }
                    }
                }
            }
        }

        //check to win game by seeing if all non-mine tiles are revealed then you have to make all mine tiles flagged making sure counter also gets to zero at tend!


        //1. clear anything already drawn
        window.clear(Color::White);

        //2. draw as many things as you need/want
        for (int i = 0; i < rows; i++) {    //draws all the tiles on the board
            for (int j = 0; j < columns; j++) {
                Sprite newSprite(tiles[j][i].getSprite());
                newSprite.setPosition(32 * j, 32 * i);
                tileSprites[j][i] = newSprite;
                window.draw(tileSprites[j][i]);

                if (tiles[j][i].getFlagStatus()) {
                    Sprite flagSprite(TextureManager::GetTexture("flag")); //change to non function
                    flagSprite.setPosition(32 * j, 32 * i);
                    window.draw(flagSprite);
                }
            }
        }

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                if (tiles[j][i].getHiddenStatus() == false && tiles[j][i].getMineStatus() == false) {
                    if (tiles[j][i].getMineCount() == 1) {
                        oneSprite.setPosition(32 * j, 32 * i);
                        window.draw(oneSprite);
                    }
                    if (tiles[j][i].getMineCount() == 2) {
                        twoSprite.setPosition(32 * j, 32 * i);
                        window.draw(twoSprite);
                    }
                    if (tiles[j][i].getMineCount() == 3) {
                        threeSprite.setPosition(32 * j, 32 * i);
                        window.draw(threeSprite);
                    }
                    if (tiles[j][i].getMineCount() == 4) {
                        fourSprite.setPosition(32 * j, 32 * i);
                        window.draw(fourSprite);
                    }
                    if (tiles[j][i].getMineCount() == 5) {
                        fiveSprite.setPosition(32 * j, 32 * i);
                        window.draw(fiveSprite);
                    }
                    if (tiles[j][i].getMineCount() == 6) {
                        sixSprite.setPosition(32 * j, 32 * i);
                        window.draw(sixSprite);
                    }
                    if (tiles[j][i].getMineCount() == 7) {
                        sevenSprite.setPosition(32 * j, 32 * i);
                        window.draw(sevenSprite);
                    }
                    if (tiles[j][i].getMineCount() == 8) {
                        eightSprite.setPosition(32 * j, 32 * i);
                        window.draw(eightSprite);
                    }
                }
            }
        }

        if (gameOver || debugMode) {    //reveals all mine pngs once game is lost or debug mode is on
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < columns; j++) {
                    if (tiles[j][i].getMineStatus()) {
                        Sprite mineSprite(TextureManager::GetTexture("mine"));
                        mineSprite.setPosition(32 * j, 32 * i);
                        window.draw(mineSprite);
                    }
                }
            }
        }


        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                int currentMines = 0;
                if (tiles[j][i].GetNeighbors()[0] != NULL) {
                    if (tiles[j][i].GetNeighbors()[0]->getMineStatus()) { //top left
                        currentMines++;
                    }
                }
                if (tiles[j][i].GetNeighbors()[1] != NULL) {
                    if (tiles[j][i].GetNeighbors()[1]->getMineStatus()) { //above
                        currentMines++;
                    }
                }
                if (tiles[j][i].GetNeighbors()[2] != NULL) {
                    if (tiles[j][i].GetNeighbors()[2]->getMineStatus()) { //top right
                        currentMines++;
                    }
                }
                if (tiles[j][i].GetNeighbors()[3] != NULL) {
                    if (tiles[j][i].GetNeighbors()[3]->getMineStatus()) { //left
                        currentMines++;
                    }
                }
                if (tiles[j][i].GetNeighbors()[4] != NULL) {
                    if (tiles[j][i].GetNeighbors()[4]->getMineStatus()) { //right
                        currentMines++;
                    }
                }
                if (tiles[j][i].GetNeighbors()[5] != NULL) {
                    if (tiles[j][i].GetNeighbors()[5]->getMineStatus()) { //bottom left
                        currentMines++;
                    }
                }
                if (tiles[j][i].GetNeighbors()[6] != NULL) {
                    if (tiles[j][i].GetNeighbors()[6]->getMineStatus()) { //below
                        currentMines++;
                    }
                }
                if (tiles[j][i].GetNeighbors()[7] != NULL) {
                    if (tiles[j][i].GetNeighbors()[7]->getMineStatus()) { //bottom right
                        currentMines++;
                    }
                }
                tiles[j][i].setMineCount(currentMines);
            }
        }

        if (gameOver) {
            winSprite.setTexture(loseTexture);
        } else if (winGame) {
            winSprite.setTexture(winTexture);
        } else {
            winSprite.setTexture(happyTexture);
        }

        if (counter < 0) {
            window.draw(negativeCounterSprite);
        }


        string counterString = to_string(counter);
        int array[counterString.size()];
        int remember = counter;
        for (int i = counterString.size() - 1; i >= 0; i--) {
            if (counter < 0) {
                counter *= -1;
            }
            array[i] = counter % 10;
            counter /= 10;
        }
        counter = remember;

        if (counterString.size() == 3) {
            firstCounterSprite.setTexture(counterTextures[array[0]]);
            secondCounterSprite.setTexture(counterTextures[array[1]]);
            thirdCounterSprite.setTexture(counterTextures[array[2]]);
        } else if (counterString.size() == 2) {
            firstCounterSprite.setTexture(counter0Texture);
            secondCounterSprite.setTexture(counterTextures[array[0]]);
            thirdCounterSprite.setTexture(counterTextures[array[1]]);
        } else if (counterString.size() == 1) {
            firstCounterSprite.setTexture(counter0Texture);
            secondCounterSprite.setTexture(counter0Texture);
            thirdCounterSprite.setTexture(counterTextures[counter]);
        } else {
            firstCounterSprite.setTexture(counter0Texture);
            secondCounterSprite.setTexture(counter0Texture);
            thirdCounterSprite.setTexture(counter0Texture);
        }

        // Render the timer
        auto elapsedD = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start);
        test.setString("Time elapsed: " + to_string(elapsedD.count()) + " seconds");
        window.draw(test);
        int tempTime = elapsedD.count() - timeRemember.asSeconds();
        if(gameOver == true){
             start = std::chrono::high_resolution_clock::now();
             auto tempTimeRest = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - start);
             tempTime = tempTimeRest.count();
        }

            if (tempTime / 60 >= 10) { //minutes
                int temp = floor(tempTime / 60);
                string minuteL = to_string(temp);
                unsigned char minuteTens = minuteL[0];
                firstTimerSprite.setTexture(timerTextures[(int) minuteTens]);
                unsigned char minuteOnes = minuteL[1];
                secondTimerSprite.setTexture(timerTextures[(int) minuteOnes]);
            } else {
                firstTimerSprite.setTexture(timerTextures[0]);
                int temp = floor(tempTime / 60);
                secondTimerSprite.setTexture(timerTextures[temp]);
            }
            //seconds
            if (tempTime > 9) {
                tempTime = tempTime % 60;
                int secondsTens = tempTime / 10;
                thirdTimerSprite.setTexture(timerTextures[(secondsTens)]);
                int secondsOnes = tempTime % 10;
                fourthTimerSprite.setTexture(timerTextures[(secondsOnes)]);
            } else {
                thirdTimerSprite.setTexture(timerTextures[0]);
                fourthTimerSprite.setTexture(timerTextures[tempTime]);
            }



        window.draw(firstCounterSprite);
        window.draw(secondCounterSprite);
        window.draw(thirdCounterSprite);

        window.draw(firstTimerSprite);
        window.draw(secondTimerSprite);
        window.draw(thirdTimerSprite);
        window.draw(fourthTimerSprite);

        window.draw(winSprite);
        window.draw(pauseBut);
        window.draw(leaderBut);

        window.draw(debugSprite);



        //3. present the results of everything that was drawn
        window.display();


    }
    TextureManager::Clear();

    return 0;
}
