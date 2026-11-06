#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace asbr {

enum class Target { Patch, Launch };

struct Mod {
    std::string id;  // nombre de la carpeta
    std::string name, author, version;
    Target target = Target::Patch;
    std::vector<std::string> files;  // CPKs del mod
    bool active = false;
};

struct Result {
    bool ok;
    std::string message;
};

class ModManager {
public:
    ModManager(std::filesystem::path modsRoot, std::filesystem::path gameData);

    void scan();
    std::vector<Mod>& mods() { return mods_; }

    Result activate(const std::string& id);
    Result deactivate(const std::string& id);

private:
    std::filesystem::path destDir(Target t) const;
    Mod* find(const std::string& id);
    // Devuelve el nombre del mod activo que ya usa ese CPK, o "" si nadie.
    std::string owner(const Mod& self, const std::string& file) const;
    void loadState();
    void saveState() const;

    std::filesystem::path root_, data_;
    std::vector<Mod> mods_;
    std::vector<std::string> activeIds_;
};

}  // namespace asbr
