#pragma once

#include <exception>
#include <string>

#include "Player.hpp"

class Player;

class AppException : public std::exception {
public:
    virtual ~AppException() = default;

    virtual const char* what() const noexcept override = 0;
};

class GameException : public AppException {
private:
    std::string message;

public:
    explicit GameException(const std::string& gameComponent, const std::string& detail);
    const char* what() const noexcept override;
};

class PlayerActionException : public AppException {
private:
    std::string message;

public:
    PlayerActionException(Player *player, const std::string& detail);
    const char* what() const noexcept override;
};

class ResourceException : public AppException {
private:
    std::string message;

public:
    explicit ResourceException(const std::string& detail);
    const char* what() const noexcept override;
};

class GraphicsException : public AppException {
private:
    std::string message;

public:
    explicit GraphicsException(const std::string& detail);
    const char* what() const noexcept override;
};