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
T *CardDeck<T>::draw() {
    if(activePile.empty() && discardedPile.empty()){
        throw GameException("CardDeck", "Deck is empty");
    }

    if(activePile.empty()){
        activePile = std::move(discardedPile);
        discardedPile.clear();
        shuffle();
    }

    T *card = activePile.back();
    activePile.pop_back();
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
