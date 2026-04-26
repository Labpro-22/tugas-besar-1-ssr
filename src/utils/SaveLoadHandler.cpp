#include "SaveLoadHandler.hpp"
#include <fstream>
#include <iostream>

SaveHandler::SaveHandler(const std::string& path)
    : savePath(path) {}

void SaveHandler::save(Game* game) {
    std::ofstream file(savePath);

    if (!file.is_open()) {
        std::cerr << "Gagal membuka file save!\n";
        return;
    }

    file << "=== SAVE GAME DATA ===\n";
    file << "Dummy save (belum implement full)\n";

    file.close();

    std::cout << "Game berhasil disimpan ke: " << savePath << "\n";
}

LoadHandler::LoadHandler(const std::string& path)
    : loadPath(path) {}

Game* LoadHandler::load() {
    std::ifstream file(loadPath);

    if (!file.is_open()) {
        std::cerr << "Gagal membuka file load!\n";
        return nullptr;
    }

    std::cout << "Game berhasil dimuat dari: " << loadPath << "\n";

    file.close();

    return nullptr; 
}