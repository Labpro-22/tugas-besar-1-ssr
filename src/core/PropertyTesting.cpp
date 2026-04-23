#include <iostream>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include "Property.hpp"
 
// ============================================================
// Helpers
// ============================================================
 
static void printSection(const string& title) {
    cout << "\n";
    cout << "========================================\n";
    cout << "  " << title << "\n";
    cout << "========================================\n";
}
 
static void pass(const string& msg) {
    cout << "  [PASS] " << msg << "\n";
}
 
static void fail(const string& msg) {
    cout << "  [FAIL] " << msg << "\n";
}
 
// ============================================================
// Factory helpers – mirrors property.txt from spec
// ============================================================
 
// Warna dummy (raylib::Color adalah struct {r,g,b,a})
static raylib::Color colorBrown()   { return {139, 90,  43,  255}; }
static raylib::Color colorLightBlue(){ return {0,   191, 255, 255}; }
static raylib::Color colorPink()    { return {255, 105, 180, 255}; }
static raylib::Color colorOrange()  { return {255, 165, 0,   255}; }
static raylib::Color colorRed()     { return {220, 20,  60,  255}; }
static raylib::Color colorYellow()  { return {255, 215, 0,   255}; }
static raylib::Color colorGreen()   { return {34,  139, 34,  255}; }
static raylib::Color colorDarkBlue(){ return {0,   0,   139, 255}; }
 
// --- Street samples from property.txt ---
 
StreetProperty* makeGarut() {
    // ID=2 GRT GARUT STREET COKLAT price=60 mortgage=40 housePrice=20 hotelPrice=50
    // rentLevels: L0=2, L1=10, L2=30, L3=90, L4=160, L5=250
    return new StreetProperty(2, "GRT", "GARUT", 60, 40,
                               colorBrown(), 20, 50,
                               {2, 10, 30, 90, 160, 250});
}
 
StreetProperty* makeTasikmalaya() {
    // ID=4 TSK TASIKMALAYA STREET COKLAT price=60 mortgage=40 housePrice=50 hotelPrice=50
    // rentLevels: L0=4, L1=20, L2=60, L3=180, L4=320, L5=450  (contoh)
    return new StreetProperty(4, "TSK", "TASIKMALAYA", 60, 40,
                               colorBrown(), 50, 50,
                               {4, 20, 60, 180, 320, 450});
}
 
StreetProperty* makeBandung() {
    // ID=32 BDG BANDUNG STREET HIJAU price=300 mortgage=250 housePrice=200 hotelPrice=200
    // rentLevels: L0=26, L1=130, L2=390, L3=900, L4=1100, L5=1275
    return new StreetProperty(32, "BDG", "BANDUNG", 300, 250,
                               colorGreen(), 200, 200,
                               {26, 130, 390, 900, 1100, 1275});
}
 
StreetProperty* makeJakarta() {
    // ID=38 JKT JAKARTA STREET BIRU_TUA price=350 mortgage=300 housePrice=200 hotelPrice=200
    // rentLevels: L0=35, L1=175, L2=500, L3=1100, L4=1300, L5=1500
    return new StreetProperty(38, "JKT", "JAKARTA", 350, 300,
                               colorDarkBlue(), 200, 200,
                               {35, 175, 500, 1100, 1300, 1500});
}
 
StreetProperty* makeIKN() {
    // ID=40 IKN IBU_KOTA_NUSANTARA STREET BIRU_TUA price=400 mortgage=350 housePrice=200 hotelPrice=200
    // rentLevels: L0=50, L1=200, L2=600, L3=1400, L4=1700, L5=2000
    return new StreetProperty(40, "IKN", "IBU_KOTA_NUSANTARA", 400, 350,
                               colorDarkBlue(), 200, 200,
                               {50, 200, 600, 1400, 1700, 2000});
}
 
// --- Railroad ---
RailroadProperty* makeGambir() {
    // ID=6 GBR STASIUN_GAMBIR RAILROAD price=200 mortgage=100
    return new RailroadProperty(6, "GBR", "STASIUN_GAMBIR", 200, 100);
}
 
RailroadProperty* makeTugu() {
    // ID=26 TUG STASIUN_TUGU RAILROAD price=200 mortgage=100
    return new RailroadProperty(26, "TUG", "STASIUN_TUGU", 200, 100);
}
 
// --- Utility ---
UtilityProperty* makePLN() {
    // ID=13 PLN PLN UTILITY price=150 mortgage=75
    return new UtilityProperty(13, "PLN", "PLN", 150, 75);
}
 
UtilityProperty* makePAM() {
    // ID=29 PAM PAM UTILITY price=150 mortgage=75
    return new UtilityProperty(29, "PAM", "PAM", 150, 75);
}
 
 
// ============================================================
// Test suites
// ============================================================
 
void testBasePropertyGetters() {
    printSection("Base Property – Getters & Initial State");
 
    StreetProperty* jkt = makeJakarta();
 
    assert(jkt->getID()   == 38);           pass("ID == 38");
    assert(jkt->getCode() == "JKT");        pass("Code == JKT");
    assert(jkt->getName() == "JAKARTA");    pass("Name == JAKARTA");
    assert(jkt->getType() == "STREET");     pass("Type == STREET");
    assert(jkt->getPrice()         == 350); pass("Price == 350");
    assert(jkt->getMortgageValue() == 300); pass("MortgageValue == 300");
    assert(jkt->getOwnerID()       == -1);  pass("OwnerID == -1 (bank)");
    assert(jkt->getStatus() == PropertyStatus::BANK); pass("Status == BANK");
 
    jkt->setOwnerID(1);
    jkt->setStatus(PropertyStatus::OWNED);
    assert(jkt->getOwnerID() == 1);                   pass("setOwnerID(1)");
    assert(jkt->getStatus()  == PropertyStatus::OWNED); pass("setStatus(OWNED)");
 
    delete jkt;
}
 
void testMortgageUnmortgage() {
    printSection("Mortgage & Unmortgage");
 
    StreetProperty* bdg = makeBandung();
    bdg->setStatus(PropertyStatus::OWNED);
 
    int cash = bdg->mortgage();
    assert(cash == 250); pass("mortgage() returns mortgageValue=250");
    assert(bdg->getStatus() == PropertyStatus::MORTGAGED); pass("Status → MORTGAGED");
 
    int cost = bdg->unmortgage();
    assert(cost == 300); pass("unmortgage() costs full price=300");
    assert(bdg->getStatus() == PropertyStatus::OWNED); pass("Status → OWNED");
 
    // Error: mortgage a BANK property
    bdg->setStatus(PropertyStatus::BANK);
    try {
        bdg->mortgage();
        fail("Should throw on BANK mortgage");
    } catch (const runtime_error&) {
        pass("Throws when mortgaging BANK property");
    }
 
    // Error: unmortgage an OWNED property
    bdg->setStatus(PropertyStatus::OWNED);
    try {
        bdg->unmortgage();
        fail("Should throw on unmortgage non-MORTGAGED");
    } catch (const runtime_error&) {
        pass("Throws when unmortgaging non-MORTGAGED property");
    }
 
    delete bdg;
}
 
void testStreetRent() {
    printSection("StreetProperty – Rent Calculation");
 
    StreetProperty* grt = makeGarut();
    grt->setStatus(PropertyStatus::OWNED);
 
    // No buildings, no monopoly (ownedCountOfType=0)
    assert(grt->getRent(0, 0) == 2);  pass("Bare land, no monopoly → rent=2");
 
    // No buildings, monopoly (ownedCountOfType != 0)
    assert(grt->getRent(0, 1) == 4);  pass("Bare land, monopoly → rent=4 (2x)");
 
    // 1 house
    grt->build();
    assert(grt->getRent(0, 0) == 10); pass("1 house → rent=10");
 
    // 2 houses
    grt->build();
    assert(grt->getRent(0, 0) == 30); pass("2 houses → rent=30");
 
    // 3 houses
    grt->build();
    assert(grt->getRent(0, 0) == 90); pass("3 houses → rent=90");
 
    // 4 houses
    grt->build();
    assert(grt->getRent(0, 0) == 160); pass("4 houses → rent=160");
 
    // Hotel
    grt->build(); // buildingCount → 5
    assert(grt->getRent(0, 0) == 250); pass("Hotel → rent=250");
 
    // Cannot build beyond hotel
    try {
        grt->build();
        fail("Should throw on building beyond hotel");
    } catch (const runtime_error&) {
        pass("Throws when building beyond hotel");
    }
 
    delete grt;
}
 
void testStreetBuildSell() {
    printSection("StreetProperty – Build & Sell Buildings");
 
    StreetProperty* jkt = makeJakarta();
    jkt->setStatus(PropertyStatus::OWNED);
 
    assert(jkt->getBuildCost() == 200); pass("getBuildCost() = housePrice=200");
    assert(jkt->canBuild());            pass("canBuild() true at level 0");
 
    // Build 4 houses
    for (int i = 1; i <= 4; i++) jkt->build();
    assert(jkt->buildingCount == 4); pass("buildingCount == 4 after 4 builds");
    assert(jkt->getBuildCost() == 200); pass("getBuildCost() = hotelPrice=200 for upgrade");
 
    // Upgrade to hotel
    jkt->build();
    assert(jkt->buildingCount == 5); pass("buildingCount == 5 (hotel)");
    assert(!jkt->canBuild());        pass("canBuild() false at hotel");
 
    // Sell hotel: refund = hotelPrice/2 = 100
    int refund = jkt->sellAllBuildings();
    assert(refund == 100);           pass("Sell hotel → refund=100 (hotelPrice/2)");
    assert(jkt->buildingCount == 4); pass("buildingCount back to 4 after selling hotel");
 
    // Sell all remaining houses
    int totalRefund = jkt->sellAllBuildings();
    // 4 houses * (housePrice/2) = 4 * 100 = 400
    assert(totalRefund == 400);      pass("sellAllBuildings() → 400 (4 × 100)");
    assert(jkt->buildingCount == 0); pass("buildingCount == 0 after sellAllBuildings");
 
    // Error: sell when no buildings
    try {
        jkt->sellAllBuildings();
        fail("Should throw when no buildings to sell");
    } catch (const runtime_error&) {
        pass("Throws when selling with no buildings");
    }
 
    delete jkt;
}
 
void testStreetTotalAndSellValue() {
    printSection("StreetProperty – getTotalValue & getSellValue");
 
    StreetProperty* ikn = makeIKN(); // price=400, housePrice=200, hotelPrice=200
    ikn->setStatus(PropertyStatus::OWNED);
 
    assert(ikn->getTotalValue() == 400); pass("getTotalValue bare = price=400");
    assert(ikn->getSellValue()  == 400); pass("getSellValue bare = price=400");
 
    // Build 2 houses
    ikn->build(); ikn->build();
    // totalValue = 400 + 2*200 = 800
    assert(ikn->getTotalValue() == 800);  pass("getTotalValue 2 houses = 800");
    // sellValue  = 400 + 2*(200/2) = 400 + 200 = 600
    assert(ikn->getSellValue()  == 600);  pass("getSellValue 2 houses = 600");
 
    // Build to hotel
    ikn->build(); ikn->build(); ikn->build(); // now 5 (hotel)
    // totalValue = 400 + 4*200 + 200 = 1400
    assert(ikn->getTotalValue() == 1400); pass("getTotalValue hotel = 1400");
    // sellValue  = 400 + 4*(100) + 100 = 900
    assert(ikn->getSellValue()  == 900);  pass("getSellValue hotel = 900");
 
    delete ikn;
}
 
void testFestival() {
    printSection("StreetProperty – Festival");
 
    StreetProperty* jkt = makeJakarta(); // baseRent at L0 = 35
    jkt->setStatus(PropertyStatus::OWNED);
 
    assert(jkt->getFestivalMultiplier() == 1);
    assert(jkt->getFestivalDuration()   == 0);
    pass("Initial: multiplier=1, duration=0");
 
    // Apply festival 1st time: multiplier 1→2, duration=3
    jkt->applyFestival();
    assert(jkt->getFestivalMultiplier() == 2);
    assert(jkt->getFestivalDuration()   == 3);
    assert(jkt->getRent(0, 0) == 35 * 2); pass("After 1st festival: rent=70, duration=3");
 
    // Decrement twice
    jkt->decrementFestival();
    jkt->decrementFestival();
    assert(jkt->getFestivalDuration() == 1); pass("After 2 decrements: duration=1");
 
    // Apply again: multiplier 2→4, duration reset to 3
    jkt->applyFestival();
    assert(jkt->getFestivalMultiplier() == 4);
    assert(jkt->getFestivalDuration()   == 3);
    assert(jkt->getRent(0, 0) == 35 * 4); pass("After 2nd festival: rent=140, duration=3");
 
    // Apply 3rd time: multiplier 4→8 (max)
    jkt->applyFestival();
    assert(jkt->getFestivalMultiplier() == 8);
    assert(jkt->getRent(0, 0) == 35 * 8); pass("After 3rd festival: rent=280 (max x8)");
 
    // Apply 4th time: multiplier stays 8, duration resets
    jkt->decrementFestival(); // duration → 2
    jkt->applyFestival();     // multiplier stays 8, duration → 3
    assert(jkt->getFestivalMultiplier() == 8);
    assert(jkt->getFestivalDuration()   == 3);
    pass("4th festival: multiplier stays 8, duration reset to 3");
 
    // Decrement to 0: auto-reset
    jkt->decrementFestival();
    jkt->decrementFestival();
    jkt->decrementFestival(); // duration → 0 → resetFestival()
    assert(jkt->getFestivalMultiplier() == 1);
    assert(jkt->getFestivalDuration()   == 0);
    pass("After duration reaches 0: festival auto-reset");
 
    delete jkt;
}
 
void testRailroadRent() {
    printSection("RailroadProperty – Rent Calculation");
 
    RailroadProperty* gbr = makeGambir();
    gbr->setStatus(PropertyStatus::OWNED);
 
    // Config: 1 railroad=25, 2=50, 3=100, 4=200 (passed as ownedCountOfType)
    assert(gbr->getRent(0, 25)  == 25);  pass("1 railroad → rent=25");
    assert(gbr->getRent(0, 50)  == 50);  pass("2 railroads → rent=50");
    assert(gbr->getRent(0, 100) == 100); pass("3 railroads → rent=100");
    assert(gbr->getRent(0, 200) == 200); pass("4 railroads → rent=200");
 
    // Railroad: getTotalValue = getSellValue = price
    assert(gbr->getTotalValue() == 200); pass("getTotalValue = price=200");
    assert(gbr->getSellValue()  == 200); pass("getSellValue = price=200");
 
    delete gbr;
}
 
void testUtilityRent() {
    printSection("UtilityProperty – Rent Calculation");
 
    UtilityProperty* pln = makePLN();
    pln->setStatus(PropertyStatus::OWNED);
 
    // Config: 1 utility → multiplier 4, 2 utilities → multiplier 10
    assert(pln->getRent(7,  4) == 28); pass("dice=7, 1 utility (mult=4) → rent=28");
    assert(pln->getRent(12, 4) == 48); pass("dice=12, 1 utility (mult=4) → rent=48");
    assert(pln->getRent(7, 10) == 70); pass("dice=7, 2 utilities (mult=10) → rent=70");
 
    delete pln;
}
 
void testCertificatePrint() {
    printSection("printCertificate Output");
 
    // Street
    {
        StreetProperty* jkt = makeJakarta();
        jkt->setOwnerID(1);
        jkt->setStatus(PropertyStatus::OWNED);
        jkt->build(); jkt->build(); // 2 houses
        jkt->applyFestival();
 
        stringstream ss;
        jkt->printCertificate(ss);
        cout << ss.str();
        pass("StreetProperty certificate printed");
        delete jkt;
    }
 
    // Railroad
    {
        RailroadProperty* gbr = makeGambir();
        gbr->setOwnerID(2);
        gbr->setStatus(PropertyStatus::OWNED);
 
        stringstream ss;
        gbr->printCertificate(ss);
        cout << ss.str();
        pass("RailroadProperty certificate printed");
        delete gbr;
    }
 
    // Utility – mortgaged
    {
        UtilityProperty* pam = makePAM();
        pam->setOwnerID(3);
        pam->setStatus(PropertyStatus::OWNED);
        pam->mortgage();
 
        stringstream ss;
        pam->printCertificate(ss);
        cout << ss.str();
        pass("UtilityProperty certificate (MORTGAGED) printed");
        delete pam;
    }
}
 
void testEdgeCases() {
    printSection("Edge Cases");
 
    // Mortgage then unmortgage cycle
    StreetProperty* tsk = makeTasikmalaya();
    tsk->setOwnerID(1);
    tsk->setStatus(PropertyStatus::OWNED);
 
    int m = tsk->mortgage();
    assert(m == 40); pass("Tasikmalaya mortgage value = 40");
    int u = tsk->unmortgage();
    assert(u == 60); pass("Tasikmalaya unmortgage cost = 60 (full price)");
    delete tsk;
 
    // Railroad mortgage
    RailroadProperty* tug = makeTugu();
    tug->setOwnerID(2);
    tug->setStatus(PropertyStatus::OWNED);
    int rm = tug->mortgage();
    assert(rm == 100); pass("Railroad mortgage value = 100");
    delete tug;
 
    // Festival on property with buildings
    StreetProperty* ikn = makeIKN(); // hotel rent = 2000
    ikn->setStatus(PropertyStatus::OWNED);
    for (int i = 0; i < 5; i++) ikn->build(); // hotel
    ikn->applyFestival(); // x2
    assert(ikn->getRent(0, 0) == 4000); pass("Hotel + festival x2 → rent=4000");
    delete ikn;
}
 
// ============================================================
// test main function
// ============================================================
 
void testProperty() {
    cout << "╔══════════════════════════════════════╗\n";
    cout << "║   NIMONSPOLI – Property Test Suite   ║\n";
    cout << "╚══════════════════════════════════════╝\n";
 
    testBasePropertyGetters();
    testMortgageUnmortgage();
    testStreetRent();
    testStreetBuildSell();
    testStreetTotalAndSellValue();
    testFestival();
    testRailroadRent();
    testUtilityRent();
    testCertificatePrint();
    testEdgeCases();
 
    cout << "\n========================================\n";
    cout << "  Semua test selesai.\n";
    cout << "========================================\n";
}
 
