#pragma once
#include <borealis.hpp>

#include "mod_manager.hpp"

class MainActivity : public brls::Activity {
public:
    CONTENT_FROM_XML_RES("activity/main.xml");

    explicit MainActivity(asbr::ModManager& mgr) : mgr_(mgr) {}
    void onContentAvailable() override;

private:
    void refresh();

    BRLS_BIND(brls::Box, list, "list");
    asbr::ModManager& mgr_;
};
