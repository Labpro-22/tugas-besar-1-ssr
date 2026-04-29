#pragma once

#include <string>
#include <vector>
#include <map>

#include "Player.hpp"


class Player;
class GameSession;


class Card {
public:
    std::string id;
    std::string cardType;

    Card(std::string id, std::string type);
    virtual ~Card() = default;
    virtual void execute(Player *player) = 0;
};



class ActionCard : public Card {
public:
    std::string actionName;

    ActionCard(std::string id, std::string type, std::string name);
    void execute(Player *player) override = 0;
};



class GoToStationCard : public ActionCard {
public:
    GoToStationCard(std::string id);
    void execute(Player *player) override;
};



class MoveBackCard : public ActionCard {
public:
    int step;

    MoveBackCard(std::string id, int step);
    void execute(Player *player) override;
};



class GoToJailCard : public ActionCard {
public:
    GoToJailCard(std::string id);
    void execute(Player *player) override;
};



class GratificationCard : public ActionCard {
public:
    int amount;

    GratificationCard(std::string id, int amount);
    void execute(Player *player) override;
};



class BirthdayGiftCard : public ActionCard {
public:
    int amount;

    BirthdayGiftCard(std::string id, int amount);
    void execute(Player *player) override;
};



class DoctorFeeCard : public ActionCard {
public:
    int amount;

    DoctorFeeCard(std::string id, int amount);
    void execute(Player *player) override;
};



class SkillCard : public Card {
public:
    std::string skillName;

    SkillCard(std::string id, std::string name);
    void execute(Player *player);
    virtual void use(Player *player) = 0;
};



class MoveCard : public SkillCard {
public:
    int distance;

    MoveCard(std::string id);
    void setDistance(int dist){ 
        distance = dist; 
        if(auto pos = skillName.find(" ("); pos != std::string::npos) skillName.erase(pos);
        skillName += " (jarak " + std::to_string(distance) + " petak)";
    }
    void use(Player *player) override;
};



class DiscountCard : public SkillCard {
public:
    int discount;
    int duration;

    DiscountCard(std::string id, int duration);
    void setDiscount(int disc){ 
        discount = disc;  
        if(auto pos = skillName.find(" ("); pos != std::string::npos) skillName.erase(pos);
        skillName += " (" + std::to_string(discount) + "% potongan harga)";
    }
    void use(Player *player) override;
};



class ShieldCard : public SkillCard {
public:
    int duration;

    ShieldCard(std::string id, int duration);
    void use(Player *player) override;
};



class TeleportCard : public SkillCard {
public:
    TeleportCard(std::string id);
    void use(Player *player) override;
};



class LassoCard : public SkillCard {
public:
    LassoCard(std::string id);
    void use(Player *player) override;
};



class DemolitionCard : public SkillCard {
private:
    struct DemolitionTarget {
        Player* owner;
        Property* prop;
    };

public:
    DemolitionCard(std::string id);
    void use(Player *player) override;
};



class FreedomCard : public SkillCard {
public:
    FreedomCard(std::string id);
    void use(Player *player) override;
};



template <typename T>
class CardDeck {
private:
    std::vector<T*> activePile;
    std::vector<T*> discardedPile;

public:
    CardDeck() = default;
    ~CardDeck();

    T* draw(bool removeCard);
    void discard(T* card);
    void shuffle();
    int size() const;
    bool isEmpty() const;
    void addCard(T* card);
    void addCardToDiscarded(T* card);
    std::vector<T*> getActivePileCards() const { return activePile; }
    std::map<std::string, int> getCardCounts() const;
};

#include "Card.tpp"
