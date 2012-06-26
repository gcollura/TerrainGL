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

#include <TextureManager.h>
#include <System.h>
#include <map>

TextureManager::TextureManager ()
    : m_textures (), m_resourceDirectories () {

}

TextureManager::~TextureManager () {

    m_textures.clear ();
    m_resourceDirectories.clear ();

}

const sf::Texture& TextureManager::getTexture (const std::string &filename) {

    // Check, whether the Texture already exists
    for (std::map<std::string, sf::Texture>::iterator it = m_textures.begin ();
         it != m_textures.end();
         ++it) {
        if (filename == it->first) {
            Console::debug (filename + " using existing Texture.");
            return it->second;
        }
    }

    // The Texture doesn't exists. Create it and save it.
    sf::Texture texture;

    // Search project's main directory
    if (texture.loadFromFile (filename)) {
        m_textures[filename] = texture;
        Console::debug (filename +  " loading Texture.");
        return m_textures[filename];
    }

    // If the Texture has still not been found, search all registered directories
    for (std::vector<std::string>::iterator it = m_resourceDirectories.begin ();
         it != m_resourceDirectories.end();
         ++it) {
        if (texture.loadFromFile((*it) + filename)) {
            m_textures[filename] = texture;
            Console::debug (filename +  " loading Texture.");
            return m_textures[filename];
        }

    }

    Console::error ("Texture was not found. It is filled with an empty Texture.");

    m_textures[filename] = texture;
    return m_textures[filename];
}

void TextureManager::deleteTexture (const sf::Texture &Texture) {

    for (std::map<std::string, sf::Texture>::iterator it = m_textures.begin();
         it != m_textures.end();
         ++it) {
        if (&Texture == &it->second) {
            m_textures.erase (it);
            return;
        }
    }
}

void TextureManager::deleteTexture (const std::string &filename) {

    std::map<std::string, sf::Texture>::iterator it = m_textures.find (filename);
    if (it != m_textures.end ())
        m_textures.erase (it);

}

void TextureManager::addResourceDirectory (const std::string &directory) {

    // Check whether the path already exists
    for (std::vector<std::string>::iterator it = m_resourceDirectories.begin ();
         it != m_resourceDirectories.end ();
         ++it) {
        // The path exists. So it isn't necessary to add id once more.
        if (directory == (*it))
            return;
    }

    // insert the directory
    m_resourceDirectories.push_back (directory);
}

void TextureManager::removeResourceDirectory (const std::string& directory) {

    for (std::vector<std::string>::iterator it = m_resourceDirectories.begin ();
         it != m_resourceDirectories.end();) {
        // The path exists. So it isn't necessary to add id once more.
        if (directory == (*it))
            it = m_resourceDirectories.erase (it);
        else
            ++it;
    }
}
