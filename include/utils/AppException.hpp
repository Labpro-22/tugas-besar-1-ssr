#pragma once

#include <exception>
#include <string>

#include "Player.hpp"

class Player;

class AppException : public std::exception {
public:
    virtual ~AppException() = default;

protected:
    virtual const char* what() const noexcept override = 0;
};



class GameException : public AppException {
private:
    std::string className;
    std::string detail;

public:
    GameException(std::string className, std::string detail) : className(className), detail(detail) {}
    const char* what() const noexcept override { return "TODO"; }
};



class PlayerActionException : public AppException {
private:
    Player* player;
    std::string detail;

public:
    PlayerActionException(Player* player, std::string detail) : player(player), detail(detail) {}
    const char* what() const noexcept override { return "TODO"; }
};



class ResourceException : public AppException {
private:
    std::string detail;

public:
    ResourceException(std::string detail) : detail(detail) {}
    const char* what() const noexcept override { return "TODO"; }
};



class GraphicsException : public AppException {
private:
    std::string detail;

public:
    GraphicsException(std::string detail) : detail(detail) {}
    const char* what() const noexcept override { return "TODO"; }
};