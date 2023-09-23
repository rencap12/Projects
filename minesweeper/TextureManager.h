#ifndef UNTITLED12_TEXTUREMANAGER_H
#define UNTITLED12_TEXTUREMANAGER_H
#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
using std::unordered_map;
using std::string;


class TextureManager {
    static unordered_map<string, sf::Texture> textures;
    static void LoadTexture(string textureName);

public:
    static sf::Texture& GetTexture(string textureName);
    static void Clear(); //call this once, at the end of main()
};


#endif //UNTITLED12_TEXTUREMANAGER_H
