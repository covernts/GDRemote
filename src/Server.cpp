#include "Server.hpp"
#include "Hacks.hpp"
#include <Geode/Geode.hpp>
#include <crow/app.h>
#include <crow/mustache.h>
#include <crow/websocket.h>

namespace gdws::Server {
    std::set<crow::websocket::connection*> clients;
    std::mutex clientsMutex;

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

    void broadcastUpdate() {
        std::lock_guard<std::mutex> lock(clientsMutex);

        crow::json::wvalue::list hacksList;
        for (const auto& hack : gdws::Hacks::m_hacks) {
            crow::json::wvalue hackCtx;
            hackCtx["name"] = hack.name;
            hackCtx["enabled"] = hack.enabled;
            hacksList.emplace_back(std::move(hackCtx));
        }

        crow::json::wvalue hacksList_t;
        hacksList_t["hacks"] = std::move(hacksList);

        for (auto client : clients) {
            client->send_text(hacksList_t.dump());
        }
    }

    void startWs() {
        crow::App<> app;

        CROW_ROUTE(app, "/")
        ([]() {
            std::string templateContent = loadTemplate((geode::Mod::get()->getResourcesDir() / "index.html").string());

            crow::json::wvalue::list hacksList;
            for (const auto& hack : gdws::Hacks::m_hacks) {
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
                for (auto& hack : gdws::Hacks::m_hacks) {
                    if (hack.name == hackName) {
                        hack.enabled = !hack.enabled;
                        broadcastUpdate();
                        return crow::response(hack.name + " is now " + (hack.enabled ? "enabled" : "disabled"));
                    }
                }
            }
            return crow::response(400, "Invalid hack name");
        });

        CROW_ROUTE(app, "/ws").websocket(&app)
        .onopen([](crow::websocket::connection& conn) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.insert(&conn);
        })
        .onclose([](crow::websocket::connection& conn, const std::string& reason) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.erase(&conn);
        });

        geode::log::info("Mod menu running on localhost:3000");
        app.port(3000).multithreaded().run();
    }
}