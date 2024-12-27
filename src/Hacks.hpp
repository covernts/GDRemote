#pragma once

namespace gdws::Hacks {
    struct Hack {
        std::string name;
        bool enabled;
    };

    extern std::vector<Hack> m_hacks;
    
    std::vector<Hack> getHacks();

    bool isEnabled(const std::string& name);
}