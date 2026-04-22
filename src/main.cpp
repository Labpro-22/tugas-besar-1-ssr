
#include "GameMenu.hpp"

int main(void){

    std::string gameTitle = "Nimonspoli Game";
    std::string gameDescription = "Fun game to play or something";
    GameMenu *menu = new GameMenu(gameTitle, gameDescription);
    
    menu->start();

}