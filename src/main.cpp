#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <crow/app.h>
#include <crow/mustache.h>

using namespace geode::prelude;
static bool noclip = false;

std::string loadTemplate(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        geode::log::error("Failed to open template file: {}", path);
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void startWs() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/noclip")
    ([]() {
        std::string templateContent = loadTemplate((Mod::get()->getResourcesDir() / "index.html").string());

        crow::mustache::context ctx;
        crow::mustache::template_t temp = templateContent;
        
        ctx["username"] = GJAccountManager::get()->m_username;
        ctx["noclip"] = noclip;
        return temp.render(ctx);
    });

    CROW_ROUTE(app, "/toggle_noclip").methods(crow::HTTPMethod::Post, crow::HTTPMethod::Get)
    ([]() {
        noclip = !noclip;
        return crow::response("Noclip is now " + std::string(noclip ? "enabled" : "disabled"));
    });

    geode::log::info("Mod menu running on localhost:3000");
    app.port(3000).multithreaded().run();
}

class $modify(PlayLayer) {
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        if (p1 == m_anticheatSpike) return PlayLayer::destroyPlayer(p0, p1);
        if (!noclip) return PlayLayer::destroyPlayer(p0, p1);
    }
};

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool s_isInit = false;
        if (!s_isInit) {
            s_isInit = true;
            std::thread(startWs).detach();
        }

        return true;
    }
};