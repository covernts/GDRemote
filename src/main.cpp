#include "Modifiers.hpp"
#include "Server.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        if (p1 == m_anticheatSpike) return PlayLayer::destroyPlayer(p0, p1);
        if (!gdremote::Modifiers::isEnabled("Noclip")) return PlayLayer::destroyPlayer(p0, p1);
    }
};

class $modify(GJBaseGameLayer) {
    virtual void update(float p0) {
        GJBaseGameLayer::update(p0);

        if (gdremote::Modifiers::isEnabled("Invisible Player")) {
            m_player1->toggleVisibility(false);
            if (m_gameState.m_isDualMode) m_player2->toggleVisibility(false);
        }
    }
};

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool s_isInit = false;
        if (!s_isInit) {
            s_isInit = true;
            Notification::create(fmt::format("Remote running on localhost:3000. Password: {}", Mod::get()->getSettingValue<std::string>("panel-pass")), NotificationIcon::Info)->show();
            std::thread(gdremote::Server::startWs).detach();
        }

        return true;
    }
};