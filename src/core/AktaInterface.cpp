#include "AktaInterface.hpp"

void AktaInterface::init() {
    SetWidth(250);
    AddElement(std::make_unique<TopBar>(this));
    AddElement(std::make_unique<Text>(this, "Akta Kepemilikan", 24, BLACK));
    AddElement(std::make_unique<Border>(this));
    AddElement(std::make_unique<Table>(this, std::vector<std::vector<std::string>>{
        {"Harga Beli", "XXX"},
        {"Nilai Gadai", "XXX"}
    }, 4, 10));
    AddElement(std::make_unique<Border>(this));
    AddElement(std::make_unique<Table>(this, std::vector<std::vector<std::string>>{
        {"Sewa (unimproved)", "XXX"},
        {"Sewa (1 rumah)", "XXX"},
        {"Sewa (2 rumah)", "XXX"},
        {"Sewa (3 rumah)", "XXX"},
        {"Sewa (4 rumah)", "XXX"},
        {"Sewa (hotel)", "XXX"}
    }, 4, 10));
    AddElement(std::make_unique<Border>(this));
    AddElement(std::make_unique<Table>(this, std::vector<std::vector<std::string>>{
        {"Harga Rumah", "XXX"},
        {"Harga Hotel", "XXX"}
    }, 4, 10));
    AddElement(std::make_unique<Border>(this));
    AddElement(std::make_unique<Table>(this, std::vector<std::vector<std::string>>{
        {"Status", "OWNED (Pemain A)"}
    }, 4, 10));
}