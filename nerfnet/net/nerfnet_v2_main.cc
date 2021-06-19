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

#include <tclap/CmdLine.h>

#include "nerfnet/net/nrf_radio_interface.h"
#include "nerfnet/util/log.h"
#include "nerfnet/util/time.h"

// A description of the program.
constexpr char kDescription[] = "Mesh networking for NRF24L01 radios.";

// The version of the program.
constexpr char kVersion[] = "0.0.1";

using nerfnet::NRFRadioInterface;
using nerfnet::RadioInterfaceV2;

int main(int argc, char** argv) {
  TCLAP::CmdLine cmd(kDescription, ' ', kVersion);
  TCLAP::ValueArg<uint32_t> address_arg("", "address",
      "The address of this station.", true, 0, "address", cmd);
  TCLAP::ValueArg<uint8_t> channel_arg("", "channel",
      "The channel to use for transmit/receive.", false, 1, "channel", cmd);
  TCLAP::ValueArg<uint16_t> ce_pin_arg("", "ce_pin",
      "Set to the index of the NRF24L01 chip-enable pin.", false, 22, "index",
      cmd);
  cmd.parse(argc, argv);

  NRFRadioInterface radio_interface(address_arg.getValue(),
      channel_arg.getValue(), ce_pin_arg.getValue());

  constexpr uint64_t kBeaconIntervalUs = 500000;
  uint64_t last_beacon_time_us = 0;
  while (1) {
    uint64_t time_now_us = nerfnet::TimeNowUs();
    if ((time_now_us - last_beacon_time_us) > kBeaconIntervalUs) {
      LOGI("beaconing");
      RadioInterfaceV2::TransmitResult result = radio_interface.Beacon();
      if (result != RadioInterfaceV2::TransmitResult::SUCCESS) {
        LOGE("Beacon failed: %d", result);
      }

      last_beacon_time_us = time_now_us;
    } else {
      RadioInterfaceV2::Frame frame;
      RadioInterfaceV2::ReceiveResult result = radio_interface.Receive(&frame);
      if (result == RadioInterfaceV2::ReceiveResult::NOT_READY) {
        nerfnet::SleepUs(100000);
      } else {
        if (result == RadioInterfaceV2::ReceiveResult::SUCCESS) {
          LOGI("Received frame: address=%" PRIu32 ", size=%zu",
              frame.address, frame.payload.size());
        } else {
          LOGE("Receive failed: %d", result);
        }
      }
    }

    nerfnet::SleepUs(100000);
  }

  return 0;
}