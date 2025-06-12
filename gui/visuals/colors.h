#pragma once
#include <vector>
#include <string>
#include <wx/colour.h>
#include <stdexcept>

// Function to convert hex color codes to wxColour objects
std::vector<wxColour> getColorPalette(const std::vector<std::string>& hexCodes) {
    std::vector<wxColour> colorPalette;
    for (const auto& hex : hexCodes) {
        if (hex.size() == 6 || (hex.size() == 7 && hex[0] == '#')) {
            std::string cleanHex = hex[0] == '#' ? hex.substr(1) : hex;

            // Convert to integer components
            unsigned long colorInt = std::stoul(cleanHex, nullptr, 16);
            unsigned char r = (colorInt >> 16) & 0xFF;
            unsigned char g = (colorInt >> 8) & 0xFF;
            unsigned char b = colorInt & 0xFF;

            // Add to palette
            colorPalette.emplace_back(r, g, b);
        } else {
            throw std::invalid_argument("Invalid hex color code: " + hex);
        }
    }
    return colorPalette;
}