#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include "esphome/components/canbus/canbus.h"
#include "esphome/components/virtual_can_bms/virtual_can_bms.h"

namespace esphome::virtual_can_bms::testing {

struct MockCanbus : canbus::Canbus {
  struct Frame {
    std::vector<uint8_t> data;
  };
  std::map<uint32_t, Frame> frames;

  bool setup_internal() override { return true; }
  canbus::Error send_message(canbus::CanFrame *frame) override {
    frames[frame->can_id].data.assign(frame->data, frame->data + frame->can_data_length_code);
    return canbus::Error::ERROR_OK;
  }
  canbus::Error read_message(canbus::CanFrame *frame) override { return canbus::Error::ERROR_NOMSG; }
};

class TestableVirtualCanBms : public VirtualCanBms {
 public:
  using VirtualCanBms::send_frame_0x0351_;
  using VirtualCanBms::send_frame_0x0355_;
  using VirtualCanBms::send_frame_0x0356_;
  using VirtualCanBms::send_frame_0x035a_;
};

}  // namespace esphome::virtual_can_bms::testing
