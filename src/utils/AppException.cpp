#include "AppException.hpp"

GameException::GameException(const std::string& detail) {
    message = "[GAME ERROR] " + detail;
}

const char* GameException::what() const noexcept {
    return message.c_str();
}

PlayerActionException::PlayerActionException(const std::string& playerName,
                                             const std::string& detail) {
    message = "[PLAYER ERROR] (" + playerName + ") " + detail;
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