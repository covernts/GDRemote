#include "Modifiers.hpp"
#include "Server.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        if (p1 == m_anticheatSpike) return PlayLayer::destroyPlayer(p0, p1);
        if (!gdws::Modifiers::isEnabled("Noclip")) return PlayLayer::destroyPlayer(p0, p1);
    }
};

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool s_isInit = false;
        if (!s_isInit) {
            s_isInit = true;
            std::thread(gdws::Server::startWs).detach();
        }

        return true;
    }
};