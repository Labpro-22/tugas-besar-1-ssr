#pragma once

#include <string>
#include <vector>
#include <map>

#include "Player.hpp"


class Player;
class GameSession;


enum class CardType {
    MOVE,
    DISCOUNT,
    SHIELD,
    TELEPORT,
    LASSO,
    DEMOLITION,
    FREEDOM,
    ACTION_STATION,
    ACTION_MOVEBACK,
    ACTION_JAIL,
    ACTION_GRAT,
    ACTION_BIRTHDAY,
    ACTION_DOCTOR
};

class Card {
public:
    std::string id;
    std::string cardType;

    Card(std::string id, std::string type);
    virtual ~Card() = default;
    virtual void execute(Player *player) = 0;

    virtual CardType getCardType() const = 0;
    virtual class SkillCard* asSkillCard() { return nullptr; }
    virtual class ActionCard* asActionCard() { return nullptr; }
    virtual class MoveCard* asMoveCard() { return nullptr; }
    virtual class DiscountCard* asDiscountCard() { return nullptr; }
    virtual class FreedomCard* asFreedomCard() { return nullptr; }

    virtual int getDistance() const { return 0; }
    virtual float getDiscountPercentage() const { return 0.0f; }
    virtual int getDuration() const { return 0; }
    virtual int getStep() const { return 0; }
    virtual int getAmount() const { return 0; }
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
    CardType getCardType() const override { return CardType::ACTION_STATION; }
    void execute(Player *player) override;
};



class MoveBackCard : public ActionCard {
public:
    int step;

    MoveBackCard(std::string id, int step);
    CardType getCardType() const override { return CardType::ACTION_MOVEBACK; }
    int getStep() const override { return step; }
    void execute(Player *player) override;
};



class GoToJailCard : public ActionCard {
public:
    GoToJailCard(std::string id);
    CardType getCardType() const override { return CardType::ACTION_JAIL; }
    void execute(Player *player) override;
};



class GratificationCard : public ActionCard {
public:
    int amount;

    GratificationCard(std::string id, int amount);
    CardType getCardType() const override { return CardType::ACTION_GRAT; }
    int getAmount() const override { return amount; }
    void execute(Player *player) override;
};



class BirthdayGiftCard : public ActionCard {
public:
    int amount;

    BirthdayGiftCard(std::string id, int amount);
    CardType getCardType() const override { return CardType::ACTION_BIRTHDAY; }
    int getAmount() const override { return amount; }
    void execute(Player *player) override;
};



class DoctorFeeCard : public ActionCard {
public:
    int amount;

    DoctorFeeCard(std::string id, int amount);
    CardType getCardType() const override { return CardType::ACTION_DOCTOR; }
    int getAmount() const override { return amount; }
    void execute(Player *player) override;
};



class SkillCard : public Card {
public:
    std::string skillName;

    SkillCard(std::string id, std::string name);
    void execute(Player *player);
    virtual void use(Player *player) = 0;
    SkillCard* asSkillCard() override { return this; }
};



class MoveCard : public SkillCard {
public:
    int distance;

    MoveCard(std::string id);
    CardType getCardType() const override { return CardType::MOVE; }
    MoveCard* asMoveCard() override { return this; }
    int getDistance() const override { return distance; }
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
    CardType getCardType() const override { return CardType::DISCOUNT; }
    DiscountCard* asDiscountCard() override { return this; }
    float getDiscountPercentage() const override { return static_cast<float>(discount); }
    int getDuration() const override { return duration; }
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
    CardType getCardType() const override { return CardType::SHIELD; }
    int getDuration() const override { return duration; }
    void use(Player *player) override;
};



class TeleportCard : public SkillCard {
public:
    TeleportCard(std::string id);
    CardType getCardType() const override { return CardType::TELEPORT; }
    void use(Player *player) override;
};



class LassoCard : public SkillCard {
public:
    LassoCard(std::string id);
    CardType getCardType() const override { return CardType::LASSO; }
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
    CardType getCardType() const override { return CardType::DEMOLITION; }
    void use(Player *player) override;
};



class FreedomCard : public SkillCard {
public:
    FreedomCard(std::string id);
    CardType getCardType() const override { return CardType::FREEDOM; }
    FreedomCard* asFreedomCard() override { return this; }
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

