#pragma once
#include <Geode/modify/PlayLayer.hpp>

class $modify(HookedPlayLayer, PlayLayer) {
    struct Fields {
        std::unordered_map<int, unsigned int> m_musicOffsets;
    };

    void pauseGame(bool unfocused);
    void resume();
};
