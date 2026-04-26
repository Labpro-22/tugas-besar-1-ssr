#include "Card.hpp"
#include "Player.hpp"
#include "GameSession.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include <iostream>

Card::Card(std::string id, std::string type) : id(id), cardType(type) {}

ActionCard::ActionCard(std::string id, std::string type, std::string name) : Card(id, type), actionName(name) {}

void ActionCard::execute(Player *player, GameSession *game) {
    std::cout << "Menjalankan Kartu Aksi: " << actionName << "\n";
}


GoToStationCard::GoToStationCard(std::string id) : ActionCard(id, "ACTION", "Pergi ke Stasiun Terdekat") {}
void GoToStationCard::execute(Player *player, GameSession *game) {
    int stationIdx = game->getBoard()->findNearestStation(player->getPosition());
    player->setPosition(stationIdx);
    std::cout << "Pemain dipindahkan ke stasiun terdekat.\n";
}


MoveBackCard::MoveBackCard(std::string id, int step) : ActionCard(id, "ACTION", "Mundur " + std::to_string(step) + " Petak"), step(step) {}
void MoveBackCard::execute(Player *player, GameSession *game) {
    player->moveForward(game->getBoard(), -step);
    std::cout << "Pemain mundur " << step << " petak.\n";
}


GoToJailCard::GoToJailCard(std::string id) : ActionCard(id, "ACTION", "Masuk Penjara") {}
void GoToJailCard::execute(Player *player, GameSession *game) {
    player->setPosition(game->getBoard()->getJailIndex());
    player->setStatus(PlayerStatus::JAILED);
    std::cout << "Pemain dikirim ke penjara!\n";
}


GratificationCard::GratificationCard(std::string id, int amount) : ActionCard(id, "ACTION", "Gratifikasi M" + std::to_string(amount)), amount(amount) {}
void GratificationCard::execute(Player *player, GameSession *game) {
    player->addMoney(amount);
    std::cout << "Pemain mendapatkan M" << amount << ".\n";
}


BirthdayGiftCard::BirthdayGiftCard(std::string id, int amount) : ActionCard(id, "ACTION", "Hadiah Ulang Tahun M" + std::to_string(amount)), amount(amount) {}
void BirthdayGiftCard::execute(Player *player, GameSession *game) {
    for (Player* p : game->getPlayers()) {
        if (p != player && p->isActive()) {
            p->deductMoney(amount);
            player->addMoney(amount);
        }
    }
    std::cout << "Pemain mendapatkan M" << amount << " dari setiap pemain lain.\n";
}


DoctorFeeCard::DoctorFeeCard(std::string id, int amount) : ActionCard(id, "ACTION", "Biaya Dokter M" + std::to_string(amount)), amount(amount) {}
void DoctorFeeCard::execute(Player *player, GameSession *game) {
    if (player->canAfford(amount)) {
        player->deductMoney(amount);
        std::cout << "Pemain membayar biaya dokter M" << amount << ".\n";
    } else {
        game->handleBankruptcy(player, nullptr, amount);
    }
}


SkillCard::SkillCard(std::string id, std::string name) : Card(id, "SKILL"), skillName(name) {}
void SkillCard::execute(Player *player, GameSession *game) {
    player->addCard(this);
}


MoveCard::MoveCard(std::string id) : SkillCard(id, "MoveCard") {}

void MoveCard::use(Player *player, GameSession *game) {
    std::cout << "MoveCard digunakan: Maju " << distance << " petak.\n";
    player->moveForward(game->getBoard(), distance);
    Tile *tile = game->getBoard()->getTile(player->getPosition());
    std::cout << "Pemain mendarat di " << tile->getName() << " (" << tile->code << ")\n";
    tile->onLanded(player, game);
}


DiscountCard::DiscountCard(std::string id) : SkillCard(id, "DiscountCard") {}
void DiscountCard::use(Player *player, GameSession *game) {
    std::cout << "DiscountCard digunakan: Diskon " << discount << "% selama " << duration << " giliran.\n";
}


ShieldCard::ShieldCard(std::string id) : SkillCard(id, "ShieldCard") {}
void ShieldCard::use(Player *player, GameSession *game) {
    std::cout << "ShieldCard digunakan: Kebal selama " << duration << " giliran.\n";
}


TeleportCard::TeleportCard(std::string id) : SkillCard(id, "TeleportCard") {}
void TeleportCard::use(Player *player, GameSession *game) {
    std::cout << "TeleportCard digunakan: Pilih kode petak tujuan: ";
    std::string code;
    std::cin >> code;
    try {
        Tile* tile = game->getBoard()->getTileByCode(code);
        player->setPosition(tile->index);
        std::cout << "Berhasil teleport ke " << tile->name << ".\n";
        tile->onLanded(player, game);
    } catch (...) {
        std::cout << "Kode petak tidak valid.\n";
    }
}


LassoCard::LassoCard(std::string id) : SkillCard(id, "LassoCard") {}
void LassoCard::use(Player *player, GameSession *game) {
    std::cout << "LassoCard digunakan: Tarik lawan ke petak Anda.\n";
    for (Player* p : game->getPlayers()) {
        if (p != player && p->isActive()) {
            p->setPosition(player->getPosition());
            std::cout << p->getUsername() << " ditarik ke petak Anda.\n";
        }
    }
}


DemolitionCard::DemolitionCard(std::string id) : SkillCard(id, "DemolitionCard") {}
void DemolitionCard::use(Player *player, GameSession *game) {
    std::cout << "DemolitionCard digunakan: Hancurkan properti lawan.\n";
}


FreedomCard::FreedomCard(std::string id) : SkillCard(id, "FreedomCard") {}
void FreedomCard::use(Player *player, GameSession *game) {
    std::cout << "FreedomCard digunakan: Keluar dari penjara gratis.\n";
    player->setStatus(PlayerStatus::ACTIVE);
    player->setJailAttempts(0);
}
