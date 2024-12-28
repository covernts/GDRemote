#include "Modifiers.hpp"

namespace gdremote::Modifiers {
    std::vector<Modifier*> m_mods = {
        new Modifier("Noclip", false, ModifierType::Checkbox),
        new Modifier("Invisible Player", false, ModifierType::Checkbox),
        new Modifier("Supress Notifications", false, ModifierType::Checkbox),
        new ButtonModifier("Close Level", []() {
            if (auto pl = PlayLayer::get()) { geode::queueInMainThread([pl]() { pl->onQuit(); }); }
        }),
        new ButtonModifier("Kill Player", []() {
            if (auto pl = PlayLayer::get()) { geode::queueInMainThread([pl]() { pl->destroyPlayer(pl->m_player1, nullptr); }); }
        }),
        new ButtonModifier("Play Sound", []() {
            geode::queueInMainThread([]() { FMODAudioEngine::get()->playEffect("explode_11.ogg"); });
        }),
    };
    
    bool isEnabled(const std::string& name) {
        for (auto mod : m_mods) {
            if (mod->getName() == name) return mod->getEnabled();
        }
        return false;
    }
}