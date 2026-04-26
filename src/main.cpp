
#include "GameApp.hpp"

int main(void){

    std::string gameTitle = "Nimonspoli Game";
    std::string gameDescription = "Fun game to play with your favourite labpro assistant";
    GameApp *app = new GameApp(gameTitle, gameDescription);
    
    app->start();

}