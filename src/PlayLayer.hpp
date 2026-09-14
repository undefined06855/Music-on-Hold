#pragma once
#include <Geode/modify/PlayLayer.hpp>

struct MusicInfo {
    static MusicInfo save(FMOD::Channel* channel);
    void load(FMOD::Channel* channel);

    unsigned int offset;
    int loopCount;
};

class $modify(HookedPlayLayer, PlayLayer) {
    struct Fields {
        std::unordered_map<int, MusicInfo> m_musicInfo;
    };

    void pauseGame(bool unfocused);
    void resume();
};
