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

        if (gdremote::Modifiers::isEnabled("Random Jump")) {
            int n = std::rand() % 50 + 1;
            if (n == 1) {
                m_player1->pushButton(PlayerButton::Jump);
                m_player1->releaseAllButtons();
                if (m_gameState.m_isDualMode) { 
                    m_player2->pushButton(PlayerButton::Jump); 
                    m_player2->releaseAllButtons();
                }
            }
        }
    }
};

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool s_isInit = false;
        if (!s_isInit) {
            s_isInit = true;
            std::thread(gdremote::Server::startWs).detach();
        }

        return true;
    }
};