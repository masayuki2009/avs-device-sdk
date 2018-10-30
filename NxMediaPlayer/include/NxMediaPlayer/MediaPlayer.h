/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on MediaPlayer.h
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

#ifndef ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_MEDIAPLAYER_H_
#define ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_MEDIAPLAYER_H_

#include <AVSCommon/SDKInterfaces/SpeakerInterface.h>
#include <AVSCommon/Utils/MediaPlayer/MediaPlayerInterface.h>
#include <AVSCommon/Utils/MediaPlayer/MediaPlayerObserverInterface.h>
#include <AVSCommon/Utils/RequiresShutdown.h>
#include <PlaylistParser/include/PlaylistParser/UrlContentToAttachmentConverter.h>
#include "NxMediaPlayer/SourceInterface.h"

namespace alexaClientSDK {
namespace mediaPlayer {

typedef std::vector<avsCommon::utils::mediaPlayer::MediaPlayerObserverInterface::TagKeyValueType> VectorOfTags;

class MediaPlayer
        : public avsCommon::utils::mediaPlayer::MediaPlayerInterface
        , public avsCommon::sdkInterfaces::SpeakerInterface
        , public playlistParser::UrlContentToAttachmentConverter::ErrorObserverInterface
        , public avsCommon::utils::RequiresShutdown
        , public std::enable_shared_from_this<MediaPlayer> {
public:

    static std::shared_ptr<MediaPlayer> create(
        std::shared_ptr<avsCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory =
            nullptr,
        avsCommon::sdkInterfaces::SpeakerInterface::Type type =
            avsCommon::sdkInterfaces::SpeakerInterface::Type::AVS_SYNCED,
        std::string name = "");

    ~MediaPlayer();

public:
    SourceId setSource(std::shared_ptr<avsCommon::avs::attachment::AttachmentReader> attachmentReader,
                       const avsCommon::utils::AudioFormat* format = nullptr) override;
    SourceId setSource(std::shared_ptr<std::istream> stream, bool repeat) override;
    SourceId setSource(const std::string& url, std::chrono::milliseconds offset = std::chrono::milliseconds::zero()) override;

    bool play(SourceId id) override;
    bool stop(SourceId id) override;
    bool pause(SourceId id) override;
    bool resume(SourceId id) override;
    uint64_t getNumBytesBuffered() override;
    std::chrono::milliseconds getOffset(SourceId id) override;
    void setObserver(std::shared_ptr<avsCommon::utils::mediaPlayer::MediaPlayerObserverInterface> observer) override;

    bool setVolume(int8_t volume) override;
    bool adjustVolume(int8_t volume) override;
    bool setMute(bool mute) override;
    bool getSpeakerSettings(avsCommon::sdkInterfaces::SpeakerInterface::SpeakerSettings* settings) override;
    avsCommon::sdkInterfaces::SpeakerInterface::Type getSpeakerType() override;

    void onError() override;
    void doShutdown() override;

    static void onEOS(void *data);
    void eos();

private:

    MediaPlayer(
        std::shared_ptr<avsCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory,
        avsCommon::sdkInterfaces::SpeakerInterface::Type type,
        std::string name);

    bool init();

    void sendPlaybackStarted();
    void sendPlaybackFinished();
    void sendPlaybackStopped();
    void tearDownTransientPipelineElements();

    void handleSetAttachmentReaderSource(
        std::shared_ptr<avsCommon::avs::attachment::AttachmentReader> reader,
        std::promise<SourceId>* promise,
        const avsCommon::utils::AudioFormat* audioFormat = nullptr);

    void cleanUpSource();

    std::shared_ptr<avsCommon::utils::mediaPlayer::MediaPlayerObserverInterface> m_playerObserver;
    std::shared_ptr<SourceInterface> m_source;
    SourceId m_currentId;

};

}  // namespace mediaPlayer
}  // namespace alexaClientSDK

#endif  // ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_MEDIAPLAYER_H_
