#include "AppException.hpp"
#include "Player.hpp"

GameException::GameException(const std::string& gameComponent, const std::string& detail){
    message = "[GAME ERROR] " + gameComponent + ": " + detail;
}

const char* GameException::what() const noexcept {
    return message.c_str();
}

PlayerActionException::PlayerActionException(Player *player,
                                             const std::string& detail) {
    message = "[PLAYER ERROR] (" + player->getUsername() + ") " + detail;
}

const char* PlayerActionException::what() const noexcept {
    return message.c_str();
}

ResourceException::ResourceException(const std::string& detail) {
    message = "[RESOURCE ERROR] " + detail;
}

const char* ResourceException::what() const noexcept {
    return message.c_str();
}

GraphicsException::GraphicsException(const std::string& detail) {
    message = "[GRAPHICS ERROR] " + detail;
}

const char* GraphicsException::what() const noexcept {
    return message.c_str();
}