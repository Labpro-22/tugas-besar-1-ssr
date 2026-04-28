#include "GameApp.hpp"
#include "Card.hpp"
#include "Player.hpp"
#include "GameSession.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include <iostream>

Card::Card(std::string id, std::string type) : id(id), cardType(type) {}

ActionCard::ActionCard(std::string id, std::string type, std::string name) : Card(id, type), actionName(name) {}

void ActionCard::execute(Player *player) {
    std::cout << "Menjalankan Kartu Aksi: " << actionName << "\n";
}


GoToStationCard::GoToStationCard(std::string id) : ActionCard(id, "ACTION", "Pergi ke Stasiun Terdekat") {}
void GoToStationCard::execute(Player *player) {
    GameSession *game = GameApp::currentSession; 
    int stationIdx = game->getBoard()->findNearestStation(player->getPosition());
    player->setPosition(stationIdx);
    std::cout << "Pemain dipindahkan ke stasiun terdekat.\n";
}


MoveBackCard::MoveBackCard(std::string id, int step) : ActionCard(id, "ACTION", "Mundur " + std::to_string(step) + " Petak"), step(step) {}
void MoveBackCard::execute(Player *player) {
    GameSession *game = GameApp::currentSession; 
    player->moveForward(game->getBoard(), -step);
    std::cout << "Pemain mundur " << step << " petak.\n";
}


GoToJailCard::GoToJailCard(std::string id) : ActionCard(id, "ACTION", "Masuk Penjara") {}
void GoToJailCard::execute(Player *player) {
    GameSession *game = GameApp::currentSession; 
    player->setPosition(game->getBoard()->getJailIndex());
    player->setStatus(PlayerStatus::JAILED);
    std::cout << "Pemain dikirim ke penjara!\n";
}


GratificationCard::GratificationCard(std::string id, int amount) : ActionCard(id, "ACTION", "Mendapat Gratifikasi M" + std::to_string(amount)), amount(amount) {}
void GratificationCard::execute(Player *player) {
    player->addMoney(amount);
    std::cout << "Pemain mendapatkan M" << amount << ".\n";
}


BirthdayGiftCard::BirthdayGiftCard(std::string id, int amount) : ActionCard(id, "ACTION", "Mendapat Hadiah Ulang Tahun M" + std::to_string(amount)), amount(amount) {}
void BirthdayGiftCard::execute(Player *player) {
    GameSession *game = GameApp::currentSession; 
    for (Player* p : game->getPlayers()) {
        if (p != player && p->isActive()) {
            if(p->canAfford(amount)){
                p->deductMoney(amount);
                player->addMoney(amount);
            }
            else {
                player->addMoney(p->getMoney());
                p->deductMoney(p->getMoney());
            }
        }
    }
    std::cout << "Pemain mendapatkan M" << amount << " dari setiap pemain lain.\n";
}


DoctorFeeCard::DoctorFeeCard(std::string id, int amount) : ActionCard(id, "ACTION", "Membayar Biaya Dokter M" + std::to_string(amount)), amount(amount) {}
void DoctorFeeCard::execute(Player *player) {
    GameSession *game = GameApp::currentSession; 

    if (player->canAfford(amount)) {
        player->deductMoney(amount);
        std::cout << "Pemain membayar biaya dokter M" << amount << ".\n";
    } else {
        game->handleBankruptcy(player, nullptr, amount);
    }
}


SkillCard::SkillCard(std::string id, std::string name) : Card(id, "SKILL"), skillName(name) {}
void SkillCard::execute(Player *player) {
    player->addCard(this);
}


MoveCard::MoveCard(std::string id) : SkillCard(id, "MoveCard") {}
void MoveCard::use(Player *player) {
    GameSession *game = GameApp::currentSession; 
    this->distance = (std::rand() % 25) + 1;
    std::cout << "MoveCard digunakan: Maju " << distance << " petak.\n";
    player->moveForward(game->getBoard(), distance);
    Tile *tile = game->getBoard()->getTile(player->getPosition());
    std::cout << "Pemain mendarat di " << tile->getName() << " (" << tile->code << ")\n";
    tile->onLanded(player);
}


DiscountCard::DiscountCard(std::string id, int duration) : SkillCard(id, "DiscountCard"), duration(duration) {}
void DiscountCard::use(Player *player) {
    discount = (std::rand() % 100);
    std::cout << "DiscountCard digunakan: Diskon " << discount << "% selama " << duration << " giliran.\n";
    player->setDiscount(discount / 100);
}


ShieldCard::ShieldCard(std::string id, int duration) : SkillCard(id, "ShieldCard"), duration(duration) {}
void ShieldCard::use(Player *player) {
    std::cout << "ShieldCard digunakan: Kebal selama " << duration << " giliran.\n";
    player->setShield(true);
}


TeleportCard::TeleportCard(std::string id) : SkillCard(id, "TeleportCard") {}
void TeleportCard::use(Player *player) {

    GameSession *game = GameApp::currentSession; 

    std::cout << "TeleportCard digunakan: Pilih kode petak tujuan: ";
    std::string code;
    std::cin >> code;
    try {
        Tile* tile = game->getBoard()->getTileByCode(code);
        player->setPosition(tile->index);
        std::cout << "Berhasil teleport ke " << tile->name << ".\n";
        tile->onLanded(player);
    } catch (AppException &) {
        std::cout << "Kode petak tidak valid.\n";
    }
}


LassoCard::LassoCard(std::string id) : SkillCard(id, "LassoCard") {}
void LassoCard::use(Player *player) {

    GameSession *game = GameApp::currentSession; 
    
    std::cout << "LassoCard digunakan: Tarik lawan ke petak Anda.\n";
    for (Player* p : game->getPlayers()) {
        if (p != player && p->isActive()) {
            p->setPosition(player->getPosition());
            std::cout << p->getUsername() << " ditarik ke petak Anda.\n";
        }
    }
}


DemolitionCard::DemolitionCard(std::string id) : SkillCard(id, "DemolitionCard") {}
void DemolitionCard::use(Player *player) {
    GameSession *game = GameApp::currentSession;
    std::cout << "DemolitionCard digunakan: Hancurkan properti lawan.\n";

    struct Target {
        Player* owner;
        Property* prop;
    };
    std::vector<Target> targets;

    for (Player* p : game->getPlayers()) {
        if (p != player && p->isActive()) {
            for (Property* prop : p->getAllProperties()) {
                targets.push_back({p, prop});
            }
        }
    }

    if (targets.empty()) {
        std::cout << "Tidak ada properti lawan yang bisa dihancurkan.\n";
        return;
    }

    std::cout << "Daftar properti lawan:\n";
    for (int i = 0; i < targets.size(); ++i) {
        std::cout << i + 1 << ". " << targets[i].owner->getUsername() << " - [" << targets[i].prop->getCode() << "] " << targets[i].prop->getName() << "\n";
    }

    std::cout << "Pilih nomor properti yang ingin dihancurkan: ";
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Input tidak valid.\n";
        return;
    }

    if (choice > 0 && choice <= static_cast<int>(targets.size())) {
        Target t = targets[choice - 1];
        Property* p = t.prop;
        Player* owner = t.owner;

        // Reset property
        p->setOwnerID(-1);
        p->setStatus(PropertyStatus::BANK);
        if (StreetProperty* sp = dynamic_cast<StreetProperty*>(p)) {
            sp->buildingCount = 0;
        }

        // Remove from owner
        owner->removeProperty(p);

        std::cout << "Properti " << p->getName() << " milik " << owner->getUsername() << " telah dihancurkan!\n";
    } else {
        std::cout << "Pilihan tidak valid.\n";
    }
}


FreedomCard::FreedomCard(std::string id) : SkillCard(id, "FreedomCard") {}
void FreedomCard::use(Player *player) {
    std::cout << "FreedomCard digunakan: Keluar dari penjara gratis.\n";
    player->setStatus(PlayerStatus::ACTIVE);
    player->setJailAttempts(0);
}
