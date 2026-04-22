#include "Tile.hpp"

Tile::Tile() : index(-1), code(""), name(""), category("") {}

Tile::Tile(int index, const std::string& code, const std::string& name, const std::string& category)
    : index(index), code(code), name(name), category(category) {}
