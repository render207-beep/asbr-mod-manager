#include "mod_manager.hpp"

#include <algorithm>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace asbr {

static bool copyFile(const fs::path& from, const fs::path& to) {
    std::ifstream in(from, std::ios::binary);
    std::ofstream out(to, std::ios::binary | std::ios::trunc);
    if (!in || !out) return false;
    constexpr size_t BUF = 1 << 20;  // 1 MiB
    auto buf = std::make_unique<char[]>(BUF);
    while (in) {
        in.read(buf.get(), BUF);
        auto n = in.gcount();
        if (n > 0) out.write(buf.get(), n);
        if (!out) return false;
    }
    return true;
}

ModManager::ModManager(fs::path modsRoot, fs::path gameData)
    : root_(std::move(modsRoot)), data_(std::move(gameData)) {
    std::error_code ec;
    fs::create_directories(root_, ec);
    loadState();
}

fs::path ModManager::destDir(Target t) const {
    return data_ / (t == Target::Patch ? "patch" : "launch");
}

void ModManager::loadState() {
    activeIds_.clear();
    std::ifstream f(root_ / "state.json");
    if (!f) return;
    try {
        auto j = json::parse(f);
        activeIds_ = j.value("active", std::vector<std::string>{});
    } catch (...) {
    }
}

void ModManager::saveState() const {
    std::ofstream f(root_ / "state.json", std::ios::trunc);
    f << json{{"active", activeIds_}}.dump(2);
}

void ModManager::scan() {
    mods_.clear();
    std::error_code ec;
    for (auto& e : fs::directory_iterator(root_, ec)) {
        if (!e.is_directory()) continue;
        Mod m;
        m.id = e.path().filename().string();
        m.name = m.id;

        std::ifstream jf(e.path() / "mod.json");
        if (jf) {
            try {
                auto j = json::parse(jf);
                m.name = j.value("name", m.id);
                m.author = j.value("author", "");
                m.version = j.value("version", "");
                m.target = j.value("target", "patch") == "launch" ? Target::Launch : Target::Patch;
                m.files = j.value("files", std::vector<std::string>{});
            } catch (...) {
            }
        }
        if (m.files.empty()) {  // sin lista: todos los .cpk de la carpeta
            for (auto& f : fs::directory_iterator(e.path(), ec))
                if (f.path().extension() == ".cpk") m.files.push_back(f.path().filename().string());
        }
        if (m.files.empty()) continue;

        // Activo solo si esta en el estado Y sus archivos siguen en destino.
        bool listed = std::find(activeIds_.begin(), activeIds_.end(), m.id) != activeIds_.end();
        bool present = listed;
        for (auto& f : m.files)
            if (!fs::exists(destDir(m.target) / f)) present = false;
        m.active = present;
        mods_.push_back(std::move(m));
    }
    std::sort(mods_.begin(), mods_.end(), [](const Mod& a, const Mod& b) { return a.name < b.name; });

    activeIds_.clear();
    for (auto& m : mods_)
        if (m.active) activeIds_.push_back(m.id);
    saveState();
}

Mod* ModManager::find(const std::string& id) {
    for (auto& m : mods_)
        if (m.id == id) return &m;
    return nullptr;
}

std::string ModManager::owner(const Mod& self, const std::string& file) const {
    for (auto& m : mods_) {
        if (m.id == self.id || !m.active || m.target != self.target) continue;
        if (std::find(m.files.begin(), m.files.end(), file) != m.files.end()) return m.name;
    }
    return "";
}

Result ModManager::activate(const std::string& id) {
    Mod* m = find(id);
    if (!m) return {false, "Mod no encontrado"};
    if (m->active) return {true, "Ya estaba activo"};

    auto dir = destDir(m->target);

    // 1) Detectar conflictos ANTES de copiar nada.
    for (auto& f : m->files) {
        if (!fs::exists(root_ / m->id / f)) return {false, "Falta el archivo " + f};
        auto o = owner(*m, f);
        if (!o.empty()) return {false, "Conflicto: " + f + " ya lo usa \"" + o + "\""};
        if (fs::exists(dir / f))
            return {false, f + " ya existe en el destino (no es de ningun mod gestionado)"};
    }

    // 2) Copiar; si algo falla, deshacer lo copiado.
    std::error_code ec;
    fs::create_directories(dir, ec);
    std::vector<fs::path> done;
    for (auto& f : m->files) {
        if (!copyFile(root_ / m->id / f, dir / f)) {
            fs::remove(dir / f, ec);
            for (auto& d : done) fs::remove(d, ec);
            return {false, "Error copiando " + f + " (SD llena?)"};
        }
        done.push_back(dir / f);
    }

    m->active = true;
    activeIds_.push_back(m->id);
    saveState();
    return {true, "Activado: " + m->name};
}

Result ModManager::deactivate(const std::string& id) {
    Mod* m = find(id);
    if (!m) return {false, "Mod no encontrado"};
    std::error_code ec;
    for (auto& f : m->files) fs::remove(destDir(m->target) / f, ec);
    m->active = false;
    activeIds_.erase(std::remove(activeIds_.begin(), activeIds_.end(), m->id), activeIds_.end());
    saveState();
    return {true, "Desactivado: " + m->name};
}

}  // namespace asbr
