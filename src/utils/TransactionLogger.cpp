#include "TransactionLogger.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

std::string LogEntry::toString() const {
    std::stringstream ss;
    ss << "[Turn " << turn << "] "
       << playerName << " | "
       << action << " | "
       << detail;
    return ss.str();
}

TransactionLogger::TransactionLogger() {}

void TransactionLogger::log(int turn, const std::string& playerName,
                           const std::string& action,
                           const std::string& detail) {
    entries.push_back({turn, playerName, action, detail});
}

std::string TransactionLogger::getRecentEntry(int n) const {
    std::stringstream ss;

    if (n <= 0 || entries.empty()) {
        return "";
    }

    int start = std::max(0, static_cast<int>(entries.size()) - n);

    for (size_t i = start; i < entries.size(); ++i) {
        ss << entries[i].toString() << "\n";
    }

    return ss.str();
}

void TransactionLogger::showRecentEntry(int n) const {
    std::cout << getRecentEntry(n);
}

int TransactionLogger::entryCount() const {
    return static_cast<int>(entries.size());
}