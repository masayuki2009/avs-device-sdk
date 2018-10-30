/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on BaseStreamSource.cpp
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

#include <AVSCommon/Utils/Logger/Logger.h>
#include <AVSCommon/AVS/Attachment/AttachmentReader.h>

#include "NxMediaPlayer/BaseStreamSource.h"

extern "C"
{
  typedef ssize_t tinyplay2_read_cb(void *buf, size_t count, void *data);
  void tinyplay2_set_read_cb(tinyplay2_read_cb *f, void *data);
}


namespace alexaClientSDK {
namespace mediaPlayer {

using namespace avsCommon::utils;
using namespace avsCommon::utils::mediaPlayer;
using namespace avsCommon::avs::attachment;

static const std::string TAG("BaseStreamSource");
#define LX(event) alexaClientSDK::avsCommon::utils::logger::LogEntry(TAG, event)

static std::string getCapsString(const AudioFormat& audioFormat) {
    std::stringstream caps;
    switch (audioFormat.encoding) {
        case AudioFormat::Encoding::LPCM:
            caps << "audio/x-raw";
            break;
        case AudioFormat::Encoding::OPUS:
            ACSDK_ERROR(LX("MediaPlayer does not handle OPUS data"));
            caps << " ";
            break;
    }

    switch (audioFormat.endianness) {
        case AudioFormat::Endianness::LITTLE:
            audioFormat.dataSigned ? caps << ",format=S" << audioFormat.sampleSizeInBits << "LE"
                                   : caps << ",format=U" << audioFormat.sampleSizeInBits << "LE";
            break;
        case AudioFormat::Endianness::BIG:
            audioFormat.dataSigned ? caps << ",format=S" << audioFormat.sampleSizeInBits << "BE"
                                   : caps << ",format=U" << audioFormat.sampleSizeInBits << "BE";
            break;
    }

    switch (audioFormat.layout) {
        case AudioFormat::Layout::INTERLEAVED:
            caps << ",layout=interleaved";
            break;
        case AudioFormat::Layout::NON_INTERLEAVED:
            caps << ",layout=non-interleaved";
            break;
    }

    caps << ",channels=" << audioFormat.numChannels;
    caps << ",rate=" << audioFormat.sampleRateHz;

    return caps.str();
}

BaseStreamSource::BaseStreamSource(const std::string& className) :
    SourceInterface(className) {
}

BaseStreamSource::~BaseStreamSource() {
    ACSDK_DEBUG9(LX("~BaseStreamSource +++ "));
}

bool BaseStreamSource::init(const AudioFormat* audioFormat) {
    std::string caps = getCapsString(*audioFormat);
    ACSDK_DEBUG9(LX("init audioformat").d("caps", caps));

    installOnReadDataHandler();
    return true;
}

void BaseStreamSource::installOnReadDataHandler() {
    tinyplay2_set_read_cb(onReadData, this);
}

ssize_t BaseStreamSource::onReadData(void *buffer, size_t size, void *data) {
    return static_cast<BaseStreamSource*>(data)->readData(buffer, size);
}

}  // namespace mediaPlayer
}  // namespace alexaClientSDK
