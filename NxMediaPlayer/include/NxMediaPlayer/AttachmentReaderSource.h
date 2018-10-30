/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on AttachmentReaderSource.h
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

#ifndef ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_ATTACHMENTREADERSOURCE_H_
#define ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_ATTACHMENTREADERSOURCE_H_

#include <NxMediaPlayer/BaseStreamSource.h>

namespace alexaClientSDK {
namespace mediaPlayer {

class AttachmentReaderSource : public BaseStreamSource {
public:
    static std::unique_ptr<AttachmentReaderSource> create(
        std::shared_ptr<avsCommon::avs::attachment::AttachmentReader> reader);

    ~AttachmentReaderSource() override;

private:
    AttachmentReaderSource(std::shared_ptr<avsCommon::avs::attachment::AttachmentReader> reader);

    bool init();

    // BaseStreamSource interface
    virtual ssize_t readData(void *buffer, size_t size) override;

    // RequiresShutdown
    void doShutdown() override{};

private:
    std::shared_ptr<avsCommon::avs::attachment::AttachmentReader> m_reader;
};

}  // namespace mediaPlayer
}  // namespace alexaClientSDK

#endif  // ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_ISTREAMSOURCE_H_
