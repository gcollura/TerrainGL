//
//  Copyright (C) 2012 Giulio Collura
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SFML/Graphics.hpp>

class TextureManager {

public:
    TextureManager ();
    virtual ~TextureManager ();

private:
    TextureManager (const TextureManager&);
    TextureManager& operator = (const TextureManager&);

public:
    const sf::Texture& getTexture (const std::string &filename);
    void deleteTexture (const sf::Texture &texture);
    void deleteTexture (const std::string &filename);
    void addResourceDirectory (const std::string &directory);
    void removeResourceDirectory (const std::string &directory);

private:
    std::map <std::string, sf::Texture> m_textures;
    std::vector <std::string> m_resourceDirectories;
};

#endif // TEXTUREMANAGER_H
