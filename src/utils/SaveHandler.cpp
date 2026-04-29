#include "SaveLoadHandler.hpp"
#include "GameSession.hpp"
#include "Board.hpp"
#include "Property.hpp"
#include "Tile.hpp"
#include "Card.hpp"
#include "TransactionLogger.hpp"
#include "AppException.hpp"
#include <fstream>
#include <sstream>

namespace {
std::string dataPath(const std::string& fileName) {
    if (fileName.rfind("data/", 0) == 0) return fileName;
    return "data/" + fileName;
}

std::string playerStatusToString(PlayerStatus status) {
    switch(status) {
        case PlayerStatus::ACTIVE: return "ACTIVE";
        case PlayerStatus::BANKRUPT: return "BANKRUPT";
        case PlayerStatus::JAILED: return "JAILED";
    }
    return "ACTIVE";
}

std::string propertyStatusToString(PropertyStatus status) {
    switch(status) {
        case PropertyStatus::BANK: return "BANK";
        case PropertyStatus::OWNED: return "OWNED";
        case PropertyStatus::MORTGAGED: return "MORTGAGED";
    }
    return "BANK";
}

std::string propertyTypeToSaveType(Property* property) {
    if (dynamic_cast<StreetProperty*>(property)) return "street";
    if (dynamic_cast<RailroadProperty*>(property)) return "railroad";
    if (dynamic_cast<UtilityProperty*>(property)) return "utility";
    return "unknown";
}

std::string skillCardToLine(SkillCard* card) {
    std::ostringstream out;
    
    std::string skillNameNoInfo = card->skillName;
    if(auto pos = skillNameNoInfo.find(" ("); pos != std::string::npos) skillNameNoInfo.erase(pos);
    out << skillNameNoInfo;

    if (auto* mc = dynamic_cast<MoveCard*>(card)) {
        out << " " << mc->distance;
    } else if (auto* dc = dynamic_cast<DiscountCard*>(card)) {
        out << " " << dc->discount;
    }

    return out.str();
}

std::string ownerNameFor(Property* property, GameSession* game) {
    if (property->getOwnerID() == -1) return "BANK";
    for (Player* player : game->getPlayers()) {
        if (player && player->getPlayerIndex() == property->getOwnerID()) {
            return player->getUsername();
        }
    }
    throw ResourceException("Cannot resolve owner for property: " + property->getCode());
}
}

SaveHandler::SaveHandler(const std::string& path) : savePath(path) {}

void SaveHandler::save(GameSession* game) {
    if (game == nullptr) {
        throw GameException("SaveHandler", "Cannot save null game session");
    }

    std::ofstream saveFile(dataPath(savePath));
    if (!saveFile.is_open()) {
        throw ResourceException("Unable to open save file for writing: " + dataPath(savePath));
    }

    std::vector<Player*>& players = game->getPlayers();
    if (players.empty()) {
        throw GameException("SaveHandler", "Cannot save game without players");
    }

    saveFile << game->getCurrentTurn() << " " << game->getMaxTurn() << "\n";
    saveFile << players.size() << "\n";

    for (Player* player : players) {
        if (player == nullptr) throw GameException("SaveHandler", "Cannot save null player");
        Tile* tile = game->getBoard()->getTile(player->getPosition());
        saveFile << player->getUsername() << " "
                 << player->getMoney() << " "
                 << tile->code << " "
                 << playerStatusToString(player->getStatus()) << "\n";

        saveFile << player->getHand().size() << "\n";
        for (SkillCard* card : player->getHand()) {
            if (card == nullptr) throw GameException("SaveHandler", "Cannot save null player card");
            saveFile << skillCardToLine(card) << "\n";
        }
    }

    for (int i = 0; i < players.size(); ++i) {
        if (i > 0) saveFile << " ";
        saveFile << players[i]->getUsername();
    }
    saveFile << "\n";

    saveFile << game->getCurrentPlayer()->getUsername() << "\n";

    std::vector<PropertyTile*> propertyTiles;
    for (int i = 0; i < game->getBoard()->getTileCount(); ++i) {
        Tile* tile = game->getBoard()->getTile(i);
        auto* propertyTile = dynamic_cast<PropertyTile*>(tile);
        if (propertyTile && propertyTile->property) propertyTiles.push_back(propertyTile);
    }

    saveFile << propertyTiles.size() << "\n";
    for (PropertyTile* propertyTile : propertyTiles) {
        Property* property = propertyTile->property;
        std::string buildingCount = "0";
        if (auto* street = dynamic_cast<StreetProperty*>(property)) {
            buildingCount = street->getBuildingCount() == 5 ? "H" : std::to_string(street->getBuildingCount());
        }

        int festivalMultiplier = property->getFestivalDuration() > 0 ? property->getFestivalMultiplier() : 1;
        int festivalDuration = property->getFestivalDuration() > 0 ? property->getFestivalDuration() : 0;

        saveFile << property->getCode() << " "
                 << propertyTypeToSaveType(property) << " "
                 << ownerNameFor(property, game) << " "
                 << propertyStatusToString(property->getStatus()) << " "
                 << festivalMultiplier << " "
                 << festivalDuration << " "
                 << buildingCount << "\n";
    }


    std::vector<SkillCard*> deckCards = game->getSkillDeck().getActivePileCards();
    saveFile << deckCards.size() << "\n";
    for (SkillCard* card : deckCards) {
        std::string skillNameNoInfo = card->skillName;
        if(auto pos = skillNameNoInfo.find(" ("); pos != std::string::npos) skillNameNoInfo.erase(pos);
        saveFile << skillNameNoInfo << "\n";
    }


    const std::vector<LogEntry>& entries = game->getLogger()->getEntries();
    saveFile << entries.size() << "\n";
    for (const LogEntry& entry : entries) {
        saveFile << entry.turn << " "
                 << entry.playerName << " "
                 << entry.action << " "
                 << entry.detail << "\n";
    }

    if (!saveFile.good()) {
        throw ResourceException("Failed while writing save file: " + dataPath(savePath));
    }
}
