#pragma once

#include <exception>
#include <string>

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
    explicit GameException(const std::string& detail);
    const char* what() const noexcept override;
};

class PlayerActionException : public AppException {
private:
    std::string message;

public:
    PlayerActionException(const std::string& playerName, const std::string& detail);
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