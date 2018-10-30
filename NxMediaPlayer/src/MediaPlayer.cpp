/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on IStreamSource.cpp
 *  Copyright 2017-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <cstring>

#include <AVSCommon/AVS/Attachment/AttachmentReader.h>
#include <AVSCommon/AVS/SpeakerConstants/SpeakerConstants.h>
#include <AVSCommon/Utils/Logger/Logger.h>
#include <AVSCommon/Utils/Memory/Memory.h>
#include <PlaylistParser/PlaylistParser.h>
#include <PlaylistParser/UrlContentToAttachmentConverter.h>

#include "NxMediaPlayer/AttachmentReaderSource.h"
#include "NxMediaPlayer/IStreamSource.h"
#include "NxMediaPlayer/MediaPlayer.h"

extern "C"
{
  void tinyplay2_start_playback(void);

  typedef void tinyplay2_eos_cb(void *data);
  void tinyplay2_set_eos_cb(tinyplay2_eos_cb *cb, void *data);
}

namespace alexaClientSDK {
namespace mediaPlayer {

using namespace avsCommon::avs::attachment;
using namespace avsCommon::avs::speakerConstants;
using namespace avsCommon::sdkInterfaces;
using namespace avsCommon::utils;
using namespace avsCommon::utils::mediaPlayer;
using namespace avsCommon::utils::memory;
using namespace avsCommon::utils::configuration;

static const std::string TAG("MediaPlayer");
#define LX(event) alexaClientSDK::avsCommon::utils::logger::LogEntry(TAG, event)


std::shared_ptr<MediaPlayer> MediaPlayer::create(
    std::shared_ptr<avsCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory,
    SpeakerInterface::Type type,
    std::string name) {
    ACSDK_DEBUG9(LX("createCalled ++++ "));
    std::shared_ptr<MediaPlayer> mediaPlayer(new MediaPlayer(contentFetcherFactory, type, name));
    if (mediaPlayer->init()) {
        return mediaPlayer;
    } else {
        return nullptr;
    }
};

MediaPlayer::~MediaPlayer() {
    ACSDK_DEBUG9(LX("~MediaPlayerCalled ++++ "));
}

void MediaPlayer::sendPlaybackStarted() {
    ACSDK_DEBUG9(LX("callingOnPlaybackStarted +++ ").d("currentId", m_currentId));
    if (m_playerObserver) {
      m_playerObserver->onPlaybackStarted(m_currentId);
    }
}

void MediaPlayer::cleanUpSource() {
    if (m_source) {
        m_source->shutdown();
    }
    m_source.reset();
}

void MediaPlayer::sendPlaybackFinished() {
    cleanUpSource();

    ACSDK_DEBUG(LX("callingOnPlaybackFinished +++").d("currentId", m_currentId));

    if (m_playerObserver) {
      m_playerObserver->onPlaybackFinished(m_currentId);
    }

    ACSDK_DEBUG(LX("sendPlaybackFinished => reset m_currentId **** "));
    m_currentId = 0;
}

void MediaPlayer::sendPlaybackStopped() {
}

void MediaPlayer::tearDownTransientPipelineElements() {
    ACSDK_DEBUG9(LX("tearDownTransientPipelineElements"));
}

void MediaPlayer::handleSetAttachmentReaderSource(
    std::shared_ptr<AttachmentReader> reader,
    std::promise<MediaPlayer::SourceId>* promise,
    const avsCommon::utils::AudioFormat* audioFormat) {
    ACSDK_DEBUG(LX("handleSetSourceCalled +++ "));

    std::shared_ptr<SourceInterface> source = AttachmentReaderSource::create(reader);

    if (!source) {
        ACSDK_ERROR(LX("handleSetAttachmentReaderSourceFailed").d("reason", "sourceIsNullptr"));
        return;
    }

    m_source = source;
    m_currentId++;

    m_source->installOnReadDataHandler();
}

MediaPlayer::SourceId MediaPlayer::setSource(
    std::shared_ptr<avsCommon::avs::attachment::AttachmentReader> reader,
    const avsCommon::utils::AudioFormat* audioFormat) {
    ACSDK_DEBUG9(LX("setSourceCalled +++++ ").d("sourceType", "AttachmentReader"));

    handleSetAttachmentReaderSource(std::move(reader), nullptr, audioFormat);
    return m_currentId;
}

MediaPlayer::SourceId MediaPlayer::setSource(std::shared_ptr<std::istream> stream, bool repeat) {
    ACSDK_DEBUG9(LX("setSourceCalled ++++ ").d("sourceType", "istream"));
    return 0;
}

MediaPlayer::SourceId MediaPlayer::setSource(const std::string& url, std::chrono::milliseconds offset) {
    ACSDK_DEBUG9(LX("setSourceForUrlCalled ++++ ").sensitive("url", url));
    return 0;
}

bool MediaPlayer::play(MediaPlayer::SourceId id) {
    ACSDK_DEBUG9(LX("playCalled ++++ "));

    if (!m_source) {
        ACSDK_ERROR(LX("playFailed").d("reason", "sourceNotSet"));
        return ERROR;
    }

    tinyplay2_start_playback();

    sendPlaybackStarted();
    return true;
}

bool MediaPlayer::stop(MediaPlayer::SourceId id) {
    ACSDK_DEBUG9(LX("stopCalled ++++ "));
    return true;
}

bool MediaPlayer::pause(MediaPlayer::SourceId id) {
    ACSDK_DEBUG9(LX("pausedCalled ++++ "));
    return true;
}

bool MediaPlayer::resume(MediaPlayer::SourceId id) {
    ACSDK_DEBUG9(LX("resumeCalled ++++ "));
    return true;
}

std::chrono::milliseconds MediaPlayer::getOffset(MediaPlayer::SourceId id) {
    ACSDK_DEBUG9(LX("getOffsetCalled ++++ "));
    return MEDIA_PLAYER_INVALID_OFFSET;
}

void MediaPlayer::setObserver(std::shared_ptr<MediaPlayerObserverInterface> observer) {
    ACSDK_DEBUG9(LX("setObserverCalled ++++ "));
    m_playerObserver = observer;
}

MediaPlayer::MediaPlayer(
    std::shared_ptr<avsCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory,
    SpeakerInterface::Type type,
    std::string name) :
      RequiresShutdown{name},
      m_playerObserver{nullptr},
      m_currentId{0} {

    ACSDK_DEBUG9(LX("Mediaplayer constructed ++++  "));
}

bool MediaPlayer::init() {
    tinyplay2_set_eos_cb(onEOS, this);
    return true;
}

bool MediaPlayer::setVolume(int8_t volume) {
    ACSDK_DEBUG9(LX("setVolumeCalled ++++  "));
    return true;
}

bool MediaPlayer::adjustVolume(int8_t delta) {
    ACSDK_DEBUG9(LX("adjustVolumeCalled ++++ "));
    return true;
}

bool MediaPlayer::setMute(bool mute) {
    ACSDK_DEBUG9(LX("setMuteCalled ++++ ").d("mute", mute));
    return true;
}

bool MediaPlayer::getSpeakerSettings(SpeakerInterface::SpeakerSettings* settings) {
    ACSDK_DEBUG9(LX("getSpeakerSettingsCalled ++++ "));
    settings->volume = 100;
    settings->mute = false;
    return true;
}

SpeakerInterface::Type MediaPlayer::getSpeakerType() {
    ACSDK_DEBUG9(LX("getSpeakerTypeCalled ++++ "));
    return SpeakerInterface::Type::AVS_SYNCED;
}

void MediaPlayer::onError() {
    ACSDK_DEBUG9(LX("onError +++++ "));
}

void MediaPlayer::doShutdown() {
}

uint64_t MediaPlayer::getNumBytesBuffered() {
    ACSDK_DEBUG9(LX("getNumBytesBuffered ++++ "));
    return 0;
}

void MediaPlayer::eos() {
    sendPlaybackFinished();
}

void MediaPlayer::onEOS(void *data) {
    return static_cast<MediaPlayer*>(data)->eos();
}

}  // namespace mediaPlayer
}  // namespace alexaClientSDK
