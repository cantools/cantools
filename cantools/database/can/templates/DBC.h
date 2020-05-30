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
    // Empty buffer constructor
    Frame(uint32_t id, const std::string& name, const uint32_t size, const bool extended, const uint32_t cycle_time, const size_t buffer_size)
        : _id(id)
        , _name(name)
        , _size(size)
        , _extended(extended)
        , _cycle_time(cycle_time)
        , _buffer_ptr{new uint8_t[buffer_size]()}
        , _buffer{_buffer_ptr.get()}
        , _buffer_size{buffer_size}
    {}

    // Move construct unique_ptr to buffer
    Frame(uint32_t id, const std::string& name, const uint32_t size, const bool extended, const uint32_t cycle_time,
          std::unique_ptr<uint8_t[]>&& other, const size_t buffer_size)
        : _id(id)
        , _name(name)
        , _size(size)
        , _extended(extended)
        , _cycle_time(cycle_time)
        , _buffer_ptr{std::move(other)}
        , _buffer{_buffer_ptr.get()}
        , _buffer_size{buffer_size}
    {}

    // Construct with raw pointer buffer, do not maintain ownership after object destruction
    Frame(uint32_t id, const std::string& name, const uint32_t size, const bool extended, const uint32_t cycle_time,
          uint8_t* buffer, const size_t buffer_size)
        : _id(id)
        , _name(name)
        , _size(size)
        , _extended(extended)
        , _cycle_time(cycle_time)
        , _buffer_ptr{nullptr}
        , _buffer{buffer}
        , _buffer_size{buffer_size}
    {}

    // Read-only span to access underlying buffer
    absl::Span<const uint8_t> buffer() const {
        return absl::Span<const uint8_t>(&_buffer[0], _buffer_size);
    }

    // Buffer to string
    std::string to_string() const {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (size_t i = 0; i < _buffer_size; ++i) {{
            oss << std::setw(2) << (unsigned int)_buffer[i];
        }}
        return oss.str();
    }

    // Clear buffer
    void clear() {
        std::fill_n(_buffer, _buffer_size, 0u);
    }

    uint32_t id() const { 
        return _id;
    }

    uint32_t PGN() const {
        if (_extended) {
            return (_id >> J1939_PGN_OFFSET) & J1939_PGN_MASK;
        } else {
            return 0;
        }
    }

    std::string name() const {
        return _name;
    }

    uint32_t size() const {
        return _size;
    }

    bool extended() const {
        return _extended;
    }

    bool standard() const {
        return !_extended;
    }

    uint32_t cycle_time() const {
        return _cycle_time;
    }

private:
    // Message ID/Frame ID
    uint32_t _id;

    // Message name
    std::string _name;

    // Message length, bytes
    uint32_t _size;

    // Extended or standard frame type
    bool _extended;

    // Message cycle time [ms]
    uint32_t _cycle_time;

protected:
    // Ownership of buffer
    std::unique_ptr<uint8_t[]> _buffer_ptr;

    // Buffer containing frame
    uint8_t* _buffer;

    // Buffer size
    size_t _buffer_size;
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
        : _buffer(buffer)
        , _name(name)
    {}

    Signal(const uint8_t* buffer, const std::string& name, const double offset,
           const double scale, const std::string& data_format, const int32_t spn)
        : _buffer(buffer)
        , _name(name)
        , _offset(offset)
        , _scale_factor(scale)
        , _data_format(data_format)
        , _spn(spn)
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
        return ((static_cast<PhysicalDataType>(value) * _scale_factor) + _offset);
    }

    /** Encode given signal by applying scaling and offset. */
    RawDataType Encode(PhysicalDataType value) const {
        return static_cast<RawDataType>((value - _offset) / _scale_factor);
    }

    /** Return string of data format, empty if none */
    std::string data_format() const {
        return _data_format;
    }
    
    /** Return SPN ID, 0 if none */
    uint32_t SPN() const {
        return _spn;
    }

protected:
    // Const pointer to buffer that signal is packed into
    const uint8_t* _buffer;

    // Signal name
    std::string _name;

    // Offset value used for encode/decode, unitless
    double _offset {0.0};

    // Scale factor used for encode/decode, unitless
    double _scale_factor {1.0};

    // Data format name
    std::string _data_format {""};

    // Suspect Parameter Number (SPN) for J1939 signal
    uint32_t _spn {0};
};

#endif  // CANTOOLS_DBC_H
