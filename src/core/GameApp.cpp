#include <iostream>

#include "GameApp.hpp"
#include "GameSession.hpp"
#include "SaveLoadHandler.hpp"

GameSession *GameApp::currentSession = nullptr;

void GameApp::printStartArt(){
    std::cout << "\n================================================================\n";
    std::cout << " Welcome to " << gameTitle << '\n';
    std::cout << gameDescription << '\n';
    std::cout << " Good Luck!\n";
    std::cout << "================================================================\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⠤⠤⠤⠀⣀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠐⢉⠴⣚⠉⣙⠢⢄⡤⢞⡂⢀⣐⢄⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡔⡤⣞⢁⠊⢀⣀⠐⢿⡄⠰⢁⡀⠈⠺⣦⢡⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⣗⣟⡸⠀⠻⡿⠃⢸⣇⢃⠿⠿⠀⠀⣽⢸⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠈⠙⢷⣴⡀⠀⠠⣪⣾⣷⡄⡀⠠⣐⢕⠁⠀\n";
    std::cout << "⠀⢰⡦⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠙⠲⡖⠓⠉⠁⠈⠉⠒⠀⠈⢸⠀⠀\n";
    std::cout << "⢶⣿⣷⣤⢀⣀⡀⠀⠀⣏⡑⠢⢄⠀⠀⠀⠈⠐⠀⠐⠀⠀⠀⠀⠀⡸⡀⠀\n";
    std::cout << "⠛⠛⠛⠟⠀⠤⠤⠌⢉⠀⠈⠓⢬⣿⣦⡤⣤⣤⠤⠤⣤⣤⣤⣤⣚⣔⣄⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⡤⠂⠀⠀⢀⠤⠤⢄⡨⠔⠒⢍⠉⢁⣯⡆\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⡗⢤⡤⣬⠀⠀⠀⢇⠀⠀⠀⠁⠀⠀⡸⢰⣿⣿⡿\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢌⡿⣽⡀⠀⠀⠈⠒⢄⡀⠀⢀⠔⠁⠈⠙⡋⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠳⢧⣠⣤⣄⣠⣀⣈⣱⡥⠤⠴⠦⠴⠃⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣿⠀⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠉⠉⠀⠈⠉⠉⠉⠀⠀⠀⠀⠀⠀\n";
}


void GameApp::printEndArt(){
    std::cout << "\n================================================================\n";
    std::cout << " Good Bye :) !\n";
    std::cout << "================================================================\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⠤⠤⠤⠀⣀⠀⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠐⢉⠴⣚⠉⣙⠢⢄⡤⢞⡂⢀⣐⢄⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡔⡤⣞⢁⠊⢀⣀⠐⢿⡄⠰⢁⡀⠈⠺⣦⢡⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⣗⣟⡸⠀⠻⡿⠃⢸⣇⢃⠿⠿⠀⠀⣽⢸⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠈⠙⢷⣴⡀⠀⠠⣪⣾⣷⡄⡀⠠⣐⢕⠁⠀\n";
    std::cout << "⠀⢰⡦⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠙⠲⡖⠓⠉⠁⠈⠉⠒⠀⠈⢸⠀⠀\n";
    std::cout << "⢶⣿⣷⣤⢀⣀⡀⠀⠀⣏⡑⠢⢄⠀⠀⠀⠈⠐⠀⠐⠀⠀⠀⠀⠀⡸⡀⠀\n";
    std::cout << "⠛⠛⠛⠟⠀⠤⠤⠌⢉⠀⠈⠓⢬⣿⣦⡤⣤⣤⠤⠤⣤⣤⣤⣤⣚⣔⣄⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⡤⠂⠀⠀⢀⠤⠤⢄⡨⠔⠒⢍⠉⢁⣯⡆\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⡗⢤⡤⣬⠀⠀⠀⢇⠀⠀⠀⠁⠀⠀⡸⢰⣿⣿⡿\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢌⡿⣽⡀⠀⠀⠈⠒⢄⡀⠀⢀⠔⠁⠈⠙⡋⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠳⢧⣠⣤⣄⣠⣀⣈⣱⡥⠤⠴⠦⠴⠃⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣿⠀⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀\n";
    std::cout << "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠉⠉⠀⠈⠉⠉⠉⠀⠀⠀⠀⠀⠀\n";
}



void GameApp::start() {
    printStartArt();

    GameConfig *gameConfig;

    std::cout << "=========================================\n";
    std::cout << "Selamat datang di " << gameTitle << "!\n";
    std::cout << gameDescription << "\n\n";
    std::cout << "=========================================\n";

    while(true){
        LoadHandler loader;

        std::string configDir;
        std::cout << "Nama folder konfigurasi (pastikan telah berada di \"config/\"): ";
        std::cin >> configDir;

        try {
            gameConfig = loader.loadConfiguration(configDir);
            break;
        }
        catch(AppException &e){
            std::cout << "Terjadi kesalahan saat melakukan loading konfigurasi: \n";
            std::cout << e.what() << '\n';
            std::cout << "Mohon ulangi prosedur...\n\n";
        }
    }


    while(true){

        std::string in;

        std::cout << "Muat permainan lewat save data? Jika tidak maka akan dimulai permainan baru (Y/N)\n";
        std::cout << ": ";
        if(!(std::cin >> in)) break;

        while(in != "Y" && in != "y" && in != "N" && in != "n"){
            std::cout << "Masukan tidak valid, tolong masukkan (Y/N).\n";
            std::cout << ": ";
            if (!(std::cin >> in)) break;
        }
        if(std::cin.eof()) break;

        bool loadFromSave = (in == "y" || in == "Y");
        while(true){
            GameSession *game;
            try {
                if(loadFromSave){
                    std::string saveDataDir;

                    std::cout << "Nama folder yang berisi save data (pastikan telah berada di \"data/\"): ";
                    std::cin >> saveDataDir;

                    game = new GameSession(saveDataDir, gameConfig);
                }
                else{
                    game = new GameSession(gameConfig);
                }
            }
            catch(AppException &e){
                std::cout << "Terjadi kesalahan saat melakukan loading data permainan: \n";
                std::cout << e.what() << '\n';
                std::cout << "Mohon ulangi prosedur...\n\n";
                continue;
            }


            try{
                currentSession = game;
                game->startGame();

                std::cout << "\n\n====================================\n";
                std::cout << "Sesi permainan telah selesai...\n";
                std::cout <<  "====================================\n\n";
            }
            catch(AppException &e){
                std::cout << "Terjadi kesalahan saat permainan berlangsung. \n";
                std::cout << e.what() << '\n';
                std::cout << "Permainan diberhentikan paksa...\n\n";
            }

            std::cout << "Lakukan sesi permainan baru? (Y/N)\n";
            std::cout << ": ";
            if(!(std::cin >> in)) break;

            while(in != "Y" && in != "y" && in != "N" && in != "n"){
                std::cout << "Masukan tidak valid, tolong masukkan (Y/N).\n";
                std::cout << ": ";
                if (!(std::cin >> in)) break;
            }
            if(in == "n" || in == "N" || std::cin.eof()) break;
        }
    }
    printEndArt();
}   