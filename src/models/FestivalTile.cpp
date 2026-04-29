#include "Tile.hpp"

#include "GameApp.hpp"
#include "AppException.hpp"
#include "Player.hpp"
#include "Property.hpp"

#include <vector>
#include <chrono>
#include <thread>

FestivalTile::FestivalTile() : Tile(), multiplier(1), duration(0) {}

FestivalTile::FestivalTile(int index, const std::string& code, const std::string& name, const std::string& category, int multiplier, int duration)
    : Tile(index, code, name, category), multiplier(multiplier), duration(duration) {}

void FestivalTile::onLanded(Player* player) {
    if (player == nullptr) {
        throw GameException("FestivalTile", "Player cannot be null.");
    }
    if (multiplier < 0.0F) {
        throw GameException("FestivalTile", "Festival multiplier cannot be negative.");
    }
    if (duration < 0) {
        throw GameException("FestivalTile", "Festival duration cannot be negative.");
    }

    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " mendarat di petak " << this->getName() << '\n';

    std::vector<StreetProperty*> eligibleProperties;
    for (Property* ownedProperty : player->getAllProperties()) {
        StreetProperty* street = dynamic_cast<StreetProperty*>(ownedProperty);
        if (street != nullptr) {
            eligibleProperties.push_back(street);
        }
    }

    if (eligibleProperties.empty()) {
        std::cout << "Tetapi pemain belum memiliki properti yang bisa dikembangkan dengan bonus festival...\n";
        return;
    }

    std::vector<int> choices;
    for (int i = 0; i < static_cast<int>(eligibleProperties.size()); ++i) {
        choices.push_back(i + 1);
    }
    std::cout << "Pilih properti untuk festival (multiplier x" << multiplier << " selama " << duration << " giliran):\n";
    for (int i = 0; i < static_cast<int>(eligibleProperties.size()); ++i) {
        std::cout << i + 1 << ". " << eligibleProperties[i]->getName() << "\n";
    }
    const int choice = player->chooseInput(choices);
    if (choice < 1 || choice > static_cast<int>(eligibleProperties.size())) {
        throw PlayerActionException(player, "Invalid festival property choice.");
    }

    std::cout << "Efek festival akan diterapkan pada properti " << eligibleProperties[choice - 1]->getName() << '\n';
    game->log("FESTIVAL", "Efek festival dicoba diterapkan pada properti " + eligibleProperties[choice - 1]->getName());
    eligibleProperties[choice - 1]->applyFestival(multiplier, duration);
}


void FestivalTile::onPassed(Player* player) {
    if (player == nullptr) {
        throw GameException("FestivalTile", "Player cannot be null.");
    }
    
    GameSession *game = GameApp::currentSession;
    game->getBoard()->printBoard();
    std::cout << "Pemain " << player->getUsername() << " melewati petak " << this->getName() << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
}

void FestivalTile::getDisplayInfo(std::stringstream& output) const {
    output << "[FestivalTile] " << name << " (" << code << ")"
           << " category=" << category
           << " multiplier=" << multiplier
           << " duration=" << duration;
}

void FestivalTile::getTileType(std::stringstream& output) const {
    output << "FESTIVAL";
}
