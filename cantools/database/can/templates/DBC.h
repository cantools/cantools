#ifndef CANTOOLS_DBC_H
#define CANTOOLS_DBC_H

#include <cstdint>
#include <iomanip>
#include <memory>
#include <string>
#include <sstream>

#include "absl/types/span.h"

/**
 * Templated functions use to decode and encode Signals
 */
template<typename data_type>
uint16_t pack_left_shift(data_type value, uint16_t shift, uint16_t mask)
{
    return static_cast<uint16_t>(static_cast<uint16_t>(value << shift) & mask);
}

template<typename data_type>
uint16_t pack_right_shift(data_type value, uint16_t shift, uint16_t mask)
{
    return static_cast<uint16_t>(static_cast<uint16_t>(value >> shift) & mask);
}

template<typename data_type>
data_type unpack_left_shift(uint16_t value, uint16_t shift, uint16_t mask)
{
    return static_cast<data_type>(static_cast<data_type>(value & mask) << shift);
}

template<typename data_type>
data_type unpack_right_shift(uint16_t value, uint16_t shift, uint16_t mask)
{
    return static_cast<data_type>(static_cast<data_type>(value & mask) >> shift);
}

/**
 * Abstract base class used to define CAN Messages
 */

static const uint32_t J1939_PGN_OFFSET = 8;
static const uint32_t J1939_PGN_MASK = 0x3FFFF;

class Frame {
public:
    /** Empty buffer constructor */
    Frame(uint32_t id, const std::string& name, const uint32_t size, const bool extended,
          const uint32_t cycle_time, const size_t buffer_size)
        : id_(id)
        , name_(name)
        , size_(size)
        , extended_(extended)
        , cycle_time_(cycle_time)
        , buffer_ptr_{new uint8_t[buffer_size]()}
        , buffer_{buffer_ptr_.get()}
        , buffer_size_{buffer_size}
    {}

    /** Move construct unique_ptr to buffer */
    Frame(uint32_t id, const std::string& name, const uint32_t size, const bool extended,
          const uint32_t cycle_time, std::unique_ptr<uint8_t[]>&& other, const size_t buffer_size)
        : id_(id)
        , name_(name)
        , size_(size)
        , extended_(extended)
        , cycle_time_(cycle_time)
        , buffer_ptr_{std::move(other)}
        , buffer_{buffer_ptr_.get()}
        , buffer_size_{buffer_size}
    {}

    /** Construct with raw pointer buffer, do not maintain ownership after object destruction */
    Frame(uint32_t id, const std::string& name, const uint32_t size, const bool extended,
          const uint32_t cycle_time, uint8_t* buffer, const size_t buffer_size)
        : id_(id)
        , name_(name)
        , size_(size)
        , extended_(extended)
        , cycle_time_(cycle_time)
        , buffer_ptr_{nullptr}
        , buffer_{buffer}
        , buffer_size_{buffer_size}
    {}

    /** Accesser to view buffer in string form. */
    std::string to_string() const {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (size_t i = 0; i < buffer_size_; ++i) {{
            oss << std::setw(2) << (unsigned int)buffer_[i];
        }}
        return oss.str();
    }

    /** Clear buffer */
    void clear() {
        std::fill_n(buffer_, buffer_size_, 0u);
    }

    /** Getter message ID. */
    uint32_t ID() const {
        return id_;
    }

    /** Getter for PGN. */
    uint32_t PGN() const {
        if (extended_) {
            return (id_ >> J1939_PGN_OFFSET) & J1939_PGN_MASK;
        } else {
            return 0;
        }
    }

    /** Getter for message name. */
    std::string name() const {
        return name_;
    }

    /** Getter for expected size of message buffer */
    uint32_t size() const {
        return size_;
    }

    /** Check if frame is extended. For J1939 message this will always be true. */
    bool extended() const {
        return extended_;
    }

    /** Check if frame is standard. For J1939 message this will always be false. */
    bool standard() const {
        return !extended_;
    }

    /** Getter for message cycle time. If cycle time is zero it is unset/not in DBC. */
    uint32_t cycle_time() const {
        return cycle_time_;
    }

    /** Getter beginning of underlying data buffer. */
    const uint8_t* buffer() {
        return &buffer_[0];
    }

    /** Getter for size of buffer */
    uint32_t buffer_size() const {
        return buffer_size_;
    }

private:
    // Message ID/Frame ID
    uint32_t id_;

    // Message name
    std::string name_;

    // Expected message length, bytes
    uint32_t size_;

    // Extended or standard frame type
    bool extended_;

    // Message cycle time [ms]
    uint32_t cycle_time_;

protected:
    // Ownership of buffer
    std::unique_ptr<uint8_t[]> buffer_ptr_;

    // Buffer containing frame
    uint8_t* buffer_;

    // Buffer size
    size_t buffer_size_;
};

/**
 * Abstract base class used to define CAN Signals
 * SignalDataType is the signal type as it exists on the CAN bus
 * PhysicalDataType is the type of the signal decoded and translated into engineering units, usually 'float' or 'double'
 */
template<typename RawDataType, typename PhysicalDataType>
class Signal {
public:
    Signal(const uint8_t* buffer, const std::string& name)
        : buffer_(buffer)
        , name_(name)
    {}

    Signal(const uint8_t* buffer, const std::string& name, const double offset,
           const double scale, const std::string& data_format, const int32_t spn)
        : buffer_(buffer)
        , name_(name)
        , offset_(offset)
        , scale_factor_(scale)
        , data_format_(data_format)
        , spn_(spn)
    {}

    /** Unpack signal from buffer */
    virtual RawDataType Raw() = 0;

    /** Confirm if raw signal within acceptable range */
    virtual bool RawInRange(const RawDataType& value) const = 0;

    /** Unpacked signal from buffer, decoded and converted to physical engineering units */
    PhysicalDataType Real() {
        return Decode(Raw());
    }

    /** Confirm if physical engineering units value in range */
    bool InRange(const PhysicalDataType& value) {
        RawDataType enable = Encode(value);
        return RawInRange(enable);
    }

    /** Decode given signal by applying scaling and offset. */
    PhysicalDataType Decode(RawDataType value) const {
        return ((static_cast<PhysicalDataType>(value) * scale_factor_) + offset_);
    }

    /** Encode given signal by applying scaling and offset. */
    RawDataType Encode(PhysicalDataType value) const {
        return static_cast<RawDataType>((value - offset_) / scale_factor_);
    }

    /** Return string of data format, empty if none */
    std::string data_format() const {
        return data_format_;
    }
    
    /** Return SPN ID, 0 if none */
    uint32_t SPN() const {
        return spn_;
    }

protected:
    // Const pointer to buffer that signal is packed into
    const uint8_t* buffer_;

    // Signal name
    std::string name_;

    // Offset value used for encode/decode, unitless
    double offset_ {0.0};

    // Scale factor used for encode/decode, unitless
    double scale_factor_ {1.0};

    // Data format name
    std::string data_format_ {""};

    // Suspect Parameter Number (SPN) for J1939 signal
    uint32_t spn_ {0};
};

#endif  // CANTOOLS_DBC_H
