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
        if (dynamic_cast<MoveCard*>(card)) counts["MoveCard"]++;
        else if (dynamic_cast<DiscountCard*>(card)) counts["DiscountCard"]++;
        else if (dynamic_cast<ShieldCard*>(card)) counts["ShieldCard"]++;
        else if (dynamic_cast<TeleportCard*>(card)) counts["TeleportCard"]++;
        else if (dynamic_cast<LassoCard*>(card)) counts["LassoCard"]++;
        else if (dynamic_cast<DemolitionCard*>(card)) counts["DemolitionCard"]++;
        else if (dynamic_cast<FreedomCard*>(card)) counts["FreedomCard"]++;
        else if (dynamic_cast<GoToStationCard*>(card)) counts["GoToStationCard"]++;
        else if (dynamic_cast<MoveBackCard*>(card)) counts["MoveBackCard"]++;
        else if (dynamic_cast<GoToJailCard*>(card)) counts["GoToJailCard"]++;
        else if (dynamic_cast<GratificationCard*>(card)) counts["GratificationCard"]++;
        else if (dynamic_cast<BirthdayGiftCard*>(card)) counts["BirthdayGiftCard"]++;
        else if (dynamic_cast<DoctorFeeCard*>(card)) counts["DoctorFeeCard"]++;
        else counts["Unknown"]++;
    }
    return counts;
}
