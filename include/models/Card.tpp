#include <algorithm>
#include <random>
#include "AppException.hpp"

template <typename T>
CardDeck<T>::~CardDeck() {
    for(T *card : activePile){
        delete card;
    }
    for(T *card : discardedPile){
        delete card;
    }
}

template <typename T>
T *CardDeck<T>::draw(bool removeCard) {
    if(activePile.empty() && discardedPile.empty()){
        throw GameException("CardDeck", "Deck is empty");
    }

    if(activePile.empty()){
        activePile = std::move(discardedPile);
        discardedPile.clear();
        shuffle();
    }

    T *card = activePile.back();
    if(removeCard) activePile.pop_back();
    return card;
}

template <typename T>
void CardDeck<T>::discard(T *card){
    if(card == nullptr) throw GameException("CardDeck", "Cannot discard null card");
    discardedPile.push_back(card);
}

template <typename T>
void CardDeck<T>::shuffle(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(activePile.begin(), activePile.end(), gen);
}

template <typename T>
int CardDeck<T>::size() const {
    return static_cast<int>(activePile.size() + discardedPile.size());
}

template <typename T>
bool CardDeck<T>::isEmpty() const {
    return activePile.empty() && discardedPile.empty();
}

template <typename T>
void CardDeck<T>::addCard(T* card) {
    if(card == nullptr){
        throw GameException("CardDeck", "Cannot add null card");
    }
    activePile.push_back(card);
}


template <typename T>
void CardDeck<T>::addCardToDiscarded(T* card) {
    if(card == nullptr){
        throw GameException("CardDeck", "Cannot add null card");
    }
    discardedPile.push_back(card);
}

template <typename T>
std::map<std::string, int> CardDeck<T>::getCardCounts() const {
    std::map<std::string, int> counts;
    for (T* card : activePile) {
        switch (card->getCardType()) {
            case CardType::MOVE: counts["MoveCard"]++; break;
            case CardType::DISCOUNT: counts["DiscountCard"]++; break;
            case CardType::SHIELD: counts["ShieldCard"]++; break;
            case CardType::TELEPORT: counts["TeleportCard"]++; break;
            case CardType::LASSO: counts["LassoCard"]++; break;
            case CardType::DEMOLITION: counts["DemolitionCard"]++; break;
            case CardType::FREEDOM: counts["FreedomCard"]++; break;
            case CardType::ACTION_STATION: counts["GoToStationCard"]++; break;
            case CardType::ACTION_MOVEBACK: counts["MoveBackCard"]++; break;
            case CardType::ACTION_JAIL: counts["GoToJailCard"]++; break;
            case CardType::ACTION_GRAT: counts["GratificationCard"]++; break;
            case CardType::ACTION_BIRTHDAY: counts["BirthdayGiftCard"]++; break;
            case CardType::ACTION_DOCTOR: counts["DoctorFeeCard"]++; break;
            default: counts["Unknown"]++; break;
        }
    }
    return counts;
}
