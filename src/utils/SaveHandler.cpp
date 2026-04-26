#include "SaveLoadHandler.hpp"
#include "GameSession.hpp"
#include <fstream>

SaveHandler::SaveHandler(const std::string& path) : savePath(path) {}

void SaveHandler::save(GameSession* game) {
}
