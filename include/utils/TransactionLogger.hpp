#pragma once

#include <vector>
#include <string>

class LogEntry {
public:
    int turn;
    std::string playerName;
    std::string action;
    std::string detail;

    std::string toString() const;
};

class TransactionLogger {
private:
    std::vector<LogEntry> entries;

public:
    TransactionLogger();

    void log(int turn, const std::string& playerName,
             const std::string& action,
             const std::string& detail);

    std::string getRecentEntry(int n) const;
    void showRecentEntry(int n) const;
    int entryCount() const;
};