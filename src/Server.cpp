#include "Server.hpp"
#include "Modifiers.hpp"
#include <Geode/Geode.hpp>
#include <crow/app.h>
#include <crow/mustache.h>
#include <crow/websocket.h>

namespace gdws::Server {
    std::set<crow::websocket::connection*> clients;
    std::mutex clientsMutex;
    std::string panelPassword = geode::Mod::get()->getSettingValue<std::string>("panel-pass");

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

    void broadcastUpdate(const std::string& notif) {
        std::lock_guard<std::mutex> lock(clientsMutex);

        crow::json::wvalue::list modsList;
        for (const auto& mod : gdws::Modifiers::m_mods) {
            crow::json::wvalue modCtx;
            modCtx["name"] = mod.name;
            modCtx["enabled"] = mod.enabled;
            modsList.emplace_back(std::move(modCtx));
        }

        crow::json::wvalue modsList_t;
        modsList_t["mods"] = std::move(modsList);
        modsList_t["notification"] = notif;

        for (auto client : clients) {
            client->send_text(modsList_t.dump());
        }
    }

    void startWs() {
        crow::App<> app;

        CROW_ROUTE(app, "/")
        ([]() {
            std::string templateContent = loadTemplate((geode::Mod::get()->getResourcesDir() / "index.html").string());

            crow::json::wvalue::list modsList;
            for (const auto& mod : gdws::Modifiers::m_mods) {
                crow::json::wvalue modCtx;
                modCtx["name"] = mod.name;
                modCtx["enabled"] = mod.enabled;
                modsList.emplace_back(std::move(modCtx));
            }
            
            crow::json::wvalue ctx;
            ctx["username"] = GJAccountManager::get()->m_username;
            ctx["mods"] = std::move(modsList);
            
            crow::mustache::template_t tmpl = templateContent;
            return tmpl.render(ctx);
        });

        CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req) {
            auto password = req.url_params.get("password");
            if (password && password == panelPassword) {
                return crow::response("success");
            }
            return crow::response(403, "Incorrect password");
        });

        CROW_ROUTE(app, "/toggle").methods(crow::HTTPMethod::Post)
        ([](const crow::request& req) {
            auto modName = req.url_params.get("mod");
            if (modName) {
                for (auto& mod : gdws::Modifiers::m_mods) {
                    if (mod.name == modName) {
                        mod.enabled = !mod.enabled;
                        auto msg = mod.name + " is now " + (mod.enabled ? "enabled" : "disabled");
                        broadcastUpdate(msg);
                        return crow::response(msg);
                    }
                }
            }
            return crow::response(400, "Invalid mod name");
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