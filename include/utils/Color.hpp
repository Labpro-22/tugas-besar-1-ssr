#pragma once

#include <string>

enum class Color {
    DEFAULT,
    PUTIH,
    HITAM,
    MERAH,
    HIJAU,
    BIRU,
    BIRU_MUDA,
    BIRU_TUA,
    KUNING,
    COKLAT,
    ORANYE,
    JINGGA,
    PINK,
    MERAH_MUDA,
    UNGU,
    ABU_ABU,
    EMAS,
    MAROON,
    MAGENTA,
    TRANSPARAN
};

inline std::string colorToString(Color color) {
    switch (color) {
        case Color::DEFAULT: return "DEFAULT";
        case Color::PUTIH: return "PUTIH";
        case Color::HITAM: return "HITAM";
        case Color::MERAH: return "MERAH";
        case Color::HIJAU: return "HIJAU";
        case Color::BIRU: return "BIRU";
        case Color::BIRU_MUDA: return "BIRU_MUDA";
        case Color::BIRU_TUA: return "BIRU_TUA";
        case Color::KUNING: return "KUNING";
        case Color::COKLAT: return "COKLAT";
        case Color::ORANYE: return "ORANYE";
        case Color::JINGGA: return "JINGGA";
        case Color::PINK: return "PINK";
        case Color::MERAH_MUDA: return "MERAH_MUDA";
        case Color::UNGU: return "UNGU";
        case Color::ABU_ABU: return "ABU_ABU";
        case Color::EMAS: return "EMAS";
        case Color::MAROON: return "MAROON";
        case Color::MAGENTA: return "MAGENTA";
        case Color::TRANSPARAN: return "TRANSPARAN";
        default: return "UNKNOWN";
    }
}

inline std::string colorToAnsi(Color color) {
    switch (color) {
        case Color::MERAH: return "\033[31m";
        case Color::HIJAU: return "\033[32m";
        case Color::KUNING: return "\033[33m";
        case Color::BIRU: return "\033[34m";
        case Color::BIRU_MUDA: return "\033[36m";
        case Color::BIRU_TUA: return "\033[34m";
        case Color::MAGENTA: return "\033[35m";
        case Color::PUTIH: return "\033[37m";
        case Color::HITAM: return "\033[30m";
        case Color::ABU_ABU: return "\033[90m";
        case Color::COKLAT: return "\033[38;5;94m";
        case Color::ORANYE:
        case Color::JINGGA: return "\033[38;5;208m";
        case Color::PINK:
        case Color::MERAH_MUDA: return "\033[38;5;213m";
        case Color::UNGU: return "\033[38;5;129m";
        case Color::EMAS: return "\033[38;5;220m";
        case Color::MAROON: return "\033[38;5;88m";
        case Color::TRANSPARAN:
        case Color::DEFAULT:
        default: return "\033[0m";
    }
}

inline std::string colorToShortName(Color color) {
    switch (color) {
        case Color::COKLAT: return "C";
        case Color::BIRU_MUDA: return "B";
        case Color::PINK: return "P";
        case Color::ORANYE:
        case Color::JINGGA: return "O";
        case Color::MERAH: return "M";
        case Color::KUNING: return "K";
        case Color::HIJAU: return "H";
        case Color::BIRU_TUA: return "T";
        case Color::ABU_ABU: return "A";
        case Color::BIRU: return "B";
        case Color::MERAH_MUDA: return "P";
        case Color::UNGU: return "U";
        case Color::EMAS: return "E";
        case Color::MAROON: return "R";
        case Color::MAGENTA: return "G";
        case Color::PUTIH: return "W";
        case Color::HITAM: return "T";
        default: return " ";
    }
}
