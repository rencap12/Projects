#include "Tile.h"

Tile::Tile() {
    hiddenTexture.loadFromFile("files/images/tile_hidden.png");
    revealedTexture.loadFromFile("files/images/tile_revealed.png");

    hidden.setTexture(hiddenTexture);
    revealed.setTexture(revealedTexture);

    isHidden = true;
    isMine = false;
    hasFlag = false;

    mineCount = 0;

    vector<Tile*> neighbors;
}


void Tile::SetNeighbors(Tile* one, Tile* two, Tile* three, Tile* four, Tile* five, Tile* six, Tile* seven, Tile* eight) {
    neighbors.push_back(one);
    neighbors.push_back(two);
    neighbors.push_back(three);
    neighbors.push_back(four);
    neighbors.push_back(five);
    neighbors.push_back(six);
    neighbors.push_back(seven);
    neighbors.push_back(eight);
}

vector<Tile*> Tile::GetNeighbors() {
    return this->neighbors;
}

sf::Sprite Tile::getSprite() {
    if (isHidden) {
        return hidden;
    }
    else {
        return revealed;
    }
}

void Tile::setMineStatus(bool change) {
    isMine = change;
}

bool Tile::getMineStatus() {
    return isMine;
}

void Tile::setFlagStatus(bool change){
    hasFlag = change;
}

bool Tile::getFlagStatus() {
    return hasFlag;
}

void Tile::setHiddenStatus(bool change){
    isHidden = change;
}

bool Tile::getHiddenStatus() {
    return isHidden;
}

int Tile::getMineCount() {
    return mineCount;
}

void Tile::setMineCount(int mineCount) {
    this->mineCount = mineCount;
}
