
#include <iostream>

#include "Tile.hpp"
#include "AppException.hpp"
#include "Player.hpp"
#include "GameApp.hpp"
#include <chrono>
#include <thread>

StartTile::StartTile() : Tile(), salary(0) {}

StartTile::StartTile(int index, const std::string& code, const std::string& name, const std::string& category, int salary)
    : Tile(index, code, name, category), salary(salary) {}

void StartTile::onPassed(Player* player) {
    if (player == nullptr) {
        throw GameException("StartTile", "Player cannot be null.");
    }
    
    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " melewati petak " << this->getName() << '\n';
    std::cout << "Pemain mendapatkan gaji sebesar " << salary << '\n';

    player->addMoney(salary);
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
}

void StartTile::onLanded(Player* player) {
    GameSession *game = GameApp::currentSession;

    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " mendarat di petak " << this->getName() << '\n';
    std::cout << "Pemain mendapatkan gaji sebesar " << salary << '\n';

    player->addMoney(salary);
}

void StartTile::getDisplayInfo(std::stringstream& output) const {
    output << "[StartTile] " << name << " (" << code << ")"
           << " category=" << category
           << " salary=" << salary;
}

void StartTile::getTileType(std::stringstream& output) const {
    output << "START";
}
