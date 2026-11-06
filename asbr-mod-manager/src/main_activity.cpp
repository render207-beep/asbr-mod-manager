#include "main_activity.hpp"

void MainActivity::onContentAvailable() { refresh(); }

void MainActivity::refresh() {
    mgr_.scan();
    list->clearViews();

    if (mgr_.mods().empty()) {
        auto* l = new brls::Label();
        l->setText("No hay mods. Pon carpetas en /switch/asbr-mods/");
        list->addView(l);
        return;
    }

    for (auto& mod : mgr_.mods()) {
        std::string title = mod.name;
        if (!mod.version.empty()) title += " v" + mod.version;
        if (!mod.author.empty()) title += "  (" + mod.author + ")";
        title += mod.target == asbr::Target::Launch ? "  [launch]" : "  [patch]";

        auto* cell = new brls::BooleanCell();
        std::string id = mod.id;
        cell->init(title, mod.active, [this, id](bool on) {
            auto r = on ? mgr_.activate(id) : mgr_.deactivate(id);
            brls::Application::notify(r.message);
            // Reconstruir la lista (tambien revierte el switch si fallo).
            brls::sync([this]() { refresh(); });
        });
        list->addView(cell);
    }
}
