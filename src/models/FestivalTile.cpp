#include "Tile.hpp"

#include "AppException.hpp"
#include "Player.hpp"
#include "Property.hpp"

#include <vector>

FestivalTile::FestivalTile() : Tile(), multiplier(1.0F), duration(0) {}

FestivalTile::FestivalTile(int index, const std::string& code, const std::string& name, const std::string& category,
                           float multiplier, int duration)
    : Tile(index, code, name, category), multiplier(multiplier), duration(duration) {}

void FestivalTile::onLanded(Player* player, GameSession* game) {
    if (player == nullptr) {
        throw GameException("FestivalTile", "Player cannot be null.");
    }
    (void)game;
    if (multiplier < 0.0F) {
        throw GameException("FestivalTile", "Festival multiplier cannot be negative.");
    }
    if (duration < 0) {
        throw GameException("FestivalTile", "Festival duration cannot be negative.");
    }

    std::vector<StreetProperty*> eligibleProperties;
    for (Property* ownedProperty : player->getAllProperties()) {
        StreetProperty* street = dynamic_cast<StreetProperty*>(ownedProperty);
        if (street != nullptr) {
            eligibleProperties.push_back(street);
        }
    }

    if (eligibleProperties.empty()) {
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

    eligibleProperties[choice - 1]->applyFestival();
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
