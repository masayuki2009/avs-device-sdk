/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on AttachmentReaderSource.cpp
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

#include <errno.h>
#include <cstring>

#include <AVSCommon/Utils/Logger/Logger.h>
#include <AVSCommon/AVS/Attachment/AttachmentReader.h>
#include <NxMediaPlayer/AttachmentReaderSource.h>

namespace alexaClientSDK {
namespace mediaPlayer {

using namespace avsCommon::avs::attachment;

static const std::string TAG("AttachmentReaderSource");
#define LX(event) alexaClientSDK::avsCommon::utils::logger::LogEntry(TAG, event)


std::unique_ptr<AttachmentReaderSource> AttachmentReaderSource::create(
    std::shared_ptr<AttachmentReader> reader) {
    std::unique_ptr<AttachmentReaderSource> result(new AttachmentReaderSource(reader));
    if (result->init()) {
        return result;
    }
    return nullptr;
};

AttachmentReaderSource::AttachmentReaderSource(std::shared_ptr<AttachmentReader> reader) :
    BaseStreamSource{"AttachmentReaderSource"},
    m_reader{reader} {
};

AttachmentReaderSource::~AttachmentReaderSource() {
}

bool AttachmentReaderSource::init() {
    return true;
}

ssize_t AttachmentReaderSource::readData(void *buffer, size_t size) {
    auto status = AttachmentReader::ReadStatus::OK;
    auto len = m_reader->read(buffer, size, &status);
    ACSDK_DEBUG9(LX("read").d("size", len).d("status", static_cast<int>(status)));

    switch (status) {
      case AttachmentReader::ReadStatus::OK_WOULDBLOCK:
        len = -1;
        set_errno(EWOULDBLOCK);
        break;

      case AttachmentReader::ReadStatus::OK_TIMEDOUT:
        len = -1;
        set_errno(ETIMEDOUT);
        break;

      case AttachmentReader::ReadStatus::ERROR_OVERRUN:
      case AttachmentReader::ReadStatus::ERROR_BYTES_LESS_THAN_WORD_SIZE:
      case AttachmentReader::ReadStatus::ERROR_INTERNAL:
        len = -1;
        set_errno(EIO);
        break;

      default:
        set_errno(0);
    }

    return len;
}

}  // namespace mediaPlayer
}  // namespace alexaClientSDK
