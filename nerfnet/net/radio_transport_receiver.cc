/*
 * Copyright 2021 Andrew Rossignol andrew.rossignol@gmail.com
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nerfnet/net/radio_transport_receiver.h"

namespace nerfnet {

RadioTransportReceiver::RadioTransportReceiver(const Clock* clock, Link* link)
    : clock_(clock), link_(link) {}

std::optional<std::string> RadioTransportReceiver::HandleFrame(
    const Link::Frame& frame) {
  HandleTimeout();

  FrameType frame_type = static_cast<FrameType>(
      frame.payload[0] & kMaskFrameType);
  if (!receive_state_.has_value() && frame_type == FrameType::BEGIN) {
    // Initialize the receiver state.
    receive_state_.emplace();
    receive_state_->address = frame.address;
    receive_state_->receive_time_us = clock_->TimeNowUs();
  } else if (receive_state_.has_value()) {
    
  }

  return std::nullopt;
}

void RadioTransportReceiver::HandleTimeout() {
  if (receive_state_.has_value()) {
    if ((clock_->TimeNowUs() - receive_state_->receive_time_us)
            > kReceiveTimeoutUs) {
      receive_state_.reset();
    }
  }
}

}  // namespace nerfnet
