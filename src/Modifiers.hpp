#pragma once

namespace gdws::Modifiers {
    struct Modifier {
        std::string name;
        bool enabled;
    };

    extern std::vector<Modifier> m_mods;

    bool isEnabled(const std::string& name);
}