#pragma once

#include <vector>
#include <string>



class LogEntry {
public:
    int turn;
    int playerID;
    std::string action;
    std::string detail;

    std::string toString();

};



class TransactionLogger {
private:
    std::vector<LogEntry> entries;

public:
    TransactionLogger();
    ~TransactionLogger() = default;

    void log(LogEntry entry);
    std::string getRecentEntry(int n);
    void showRecentEntry(int n);
    void showRecentEntry(std::stringstream& output, int n);
    int entryCount();
};