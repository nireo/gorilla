#pragma once

#include <cstdint>
#include <vector>

class BitWriter {
public:
  explicit BitWriter(std::vector<uint8_t> &buffer)
      : buffer_(buffer), bit_count_(8), current_byte_(0) {}

  void write_bit(bool bit) {
    if (bit) {
      current_byte_ |= (1 << (7 - bit_count_));
    }
    bit_count_++;

    if (bit_count_ == 8) {
      buffer_.push_back(current_byte_);
      current_byte_ = 0;
      bit_count_ = 0;
    }
  }

  void write_bits(uint64_t value, int num_bits) {
    for (int i = num_bits - 1; i >= 0; i--) {
      write_bit((value >> i) & 1);
    }
  }

  void write_byte(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
      write_bit((byte >> i) & 1);
    }
  }

  void flush(bool padding_bit = false) {
    while (bit_count_ != 0) {
      write_bit(padding_bit);
    }
  }

private:
  std::vector<uint8_t> &buffer_;
  uint8_t bit_count_;
  uint8_t current_byte_;
};

class Compressor {
  explicit Compressor(std::vector<uint8_t> &writer, uint32_t header)
      : writer_(BitWriter(writer)), header_(static_cast<int32_t>(header)),
        leading_zeros_(255), t_(0), t_delta_(0), trailing_zeros_(0) {
    writer_.write_bits(static_cast<uint64_t>(header), 32);
  }

private:
  BitWriter writer_;
  int32_t header_;
  int32_t t_;
  int32_t t_delta_;
  uint8_t leading_zeros_;
  uint8_t trailing_zeros_;
  uint64_t value_;
};