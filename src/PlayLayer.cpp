#include "PlayLayer.hpp"

MusicInfo MusicInfo::save(FMOD::Channel* channel) {
    MusicInfo ret;
    channel->getPosition(&ret.offset, FMOD_TIMEUNIT_MS);
    channel->getLoopCount(&ret.loopCount);
    return ret;
}

void MusicInfo::load(FMOD::Channel* channel) {
    channel->setPosition(this->offset, FMOD_TIMEUNIT_MS);
    channel->setLoopCount(this->loopCount);
}

void HookedPlayLayer::pauseGame(bool unfocused) {
    auto fields = m_fields.self();
    auto engine = FMODAudioEngine::get();

    float cutoff = geode::Mod::get()->getSettingValue<uint64_t>("low-pass-cutoff");
    int gain = geode::Mod::get()->getSettingValue<uint64_t>("fader-gain");

    for (auto& [ id, music ] : engine->m_fmodMusic) {
        auto channel = engine->channelForChannelID(music.m_channelID);
        fields->m_musicInfo[id] = MusicInfo::save(channel);

        channel->setLoopCount(-1);

        FMOD::DSP* dsp;
        engine->m_system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &dsp);
        dsp->setParameterFloat(FMOD_DSP_LOWPASS_RESONANCE, 0.f);
        dsp->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, cutoff);
        channel->addDSP(FMOD_CHANNELCONTROL_DSP_HEAD, dsp);

        engine->m_system->createDSPByType(FMOD_DSP_TYPE_FADER, &dsp);
        dsp->setParameterFloat(FMOD_DSP_FADER_GAIN, gain);
        channel->addDSP(FMOD_CHANNELCONTROL_DSP_HEAD, dsp);
    }

    PlayLayer::pauseGame(unfocused);

    if (!m_playerDied) this->resumeAudio();
}

void HookedPlayLayer::resume() {
    auto fields = m_fields.self();
    auto engine = FMODAudioEngine::get();

    for (auto& [ id, music ] : engine->m_fmodMusic) {
        auto channel = engine->channelForChannelID(music.m_channelID);
        fields->m_musicInfo.at(id).load(channel);

        FMOD::DSP* dsp;
        channel->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &dsp);
        channel->removeDSP(dsp);
        dsp->release();

        channel->getDSP(FMOD_CHANNELCONTROL_DSP_HEAD, &dsp);
        channel->removeDSP(dsp);
        dsp->release();
    }

    fields->m_musicInfo.clear();

    PlayLayer::resume();
}
