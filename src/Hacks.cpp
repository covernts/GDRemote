#include "Hacks.hpp"

namespace gdws::Hacks {
    std::vector<Hack> m_hacks = {
        {"Noclip", false}
    };

    bool isEnabled(const std::string& name) {
        for (const auto& hack : m_hacks) {
            if (hack.name == name) return hack.enabled;
        }
        return false;
    }
}