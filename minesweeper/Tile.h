#ifndef UNTITLED12_TILE_H
#define UNTITLED12_TILE_H
#include <SFML/Graphics.hpp>
#include <vector>
using namespace std;

class Tile {
private:
    sf::Sprite hidden;
    sf::Sprite revealed;
    sf::Texture hiddenTexture;
    sf::Texture revealedTexture;
    bool isHidden;
    bool isMine;
    bool hasFlag;
    int mineCount;
    vector<Tile*> neighbors;

public:
    Tile();
    sf::Sprite getSprite();
    void setMineCount(int mineCount);
    int getMineCount();
    void setMineStatus(bool change);
    bool getMineStatus();
    void setFlagStatus(bool change);
    bool getFlagStatus();
    void setHiddenStatus(bool change);
    bool getHiddenStatus();
    void SetNeighbors(Tile* one, Tile* two, Tile* three, Tile* four, Tile* five, Tile* six, Tile* seven, Tile* eight);
    vector<Tile*> GetNeighbors();
};


#endif //UNTITLED12_TILE_H
