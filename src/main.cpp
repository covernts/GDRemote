#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <crow/app.h>
#include <crow/mustache.h>

using namespace geode::prelude;

struct Hack {
    std::string name;
    bool enabled;
};

std::vector<Hack> hacks = {
    {"Noclip", false},
};

bool isEnabled(std::string name) {
    for (auto hack : hacks) { if (hack.name == name) return hack.enabled; }
    return false;
}

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

    CROW_ROUTE(app, "/")
    ([]() {
        std::string templateContent = loadTemplate((Mod::get()->getResourcesDir() / "index.html").string());

        crow::json::wvalue::list hacksList;
        for (const auto& hack : hacks) {
            crow::json::wvalue hackCtx;
            hackCtx["name"] = hack.name;
            hackCtx["enabled"] = hack.enabled;
            hacksList.emplace_back(std::move(hackCtx));
        }
        
        crow::json::wvalue ctx;
        ctx["username"] = GJAccountManager::get()->m_username;
        ctx["hacks"] = std::move(hacksList);
        
        crow::mustache::template_t tmpl = templateContent;
        return tmpl.render(ctx);
    });

    CROW_ROUTE(app, "/toggle_hack").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        auto hackName = req.url_params.get("hack");
        if (hackName) {
            for (auto& hack : hacks) {
                if (hack.name == hackName) {
                    hack.enabled = !hack.enabled;
                    return crow::response(hack.name + " is now " + (hack.enabled ? "enabled" : "disabled"));
                }
            }
        }
        return crow::response(400, "Invalid hack name");
    });

    geode::log::info("Mod menu running on localhost:3000");
    app.port(3000).multithreaded().run();
}

class $modify(PlayLayer) {
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        if (p1 == m_anticheatSpike) return PlayLayer::destroyPlayer(p0, p1);
        if (!isEnabled("Noclip")) return PlayLayer::destroyPlayer(p0, p1);
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