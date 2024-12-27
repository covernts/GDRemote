#include "Modifiers.hpp"

namespace gdws::Modifiers {
    std::vector<Modifier> m_mods = {
        {"Noclip", false}
    };
    
    bool isEnabled(const std::string& name) {
        for (const auto& mod : m_mods) {
            if (mod.name == name) return mod.enabled;
        }
        return false;
    }
}