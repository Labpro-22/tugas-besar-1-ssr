
#include <string>

class GameMenu {
    std::string gameTitle;
    std::string gameDescription;

    void printStartArt();
    void printEndArt();

public:
    GameMenu(std::string title, std::string desc) : gameTitle(title), gameDescription(desc) {}
    ~GameMenu();

    void start();
};