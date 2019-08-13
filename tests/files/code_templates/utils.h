/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This file was generated by cantools version ${ctx.version} ${ctx.date}.
 */

#ifndef ${ctx.include_guard}
#define ${ctx.include_guard}

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef EINVAL
#    define EINVAL 22
#endif

/* Frame ids. */
% for message in ctx.messages:
#define ${message.struct_name.upper()}_FRAME_ID (0x${"%02x" % message.frame_id}u)
% endfor

/* Frame lengths in bytes. */
% for message in ctx.messages:
#define ${message.struct_name.upper()}_LENGTH (${message.length}u)
% endfor

/* Extended or standard frame types. */
% for message in ctx.messages:
#define ${message.struct_name.upper()}_IS_EXTENDED (${int(message.is_extended_frame)})
% endfor

/* Frame cycle times in milliseconds. */
% for message in ctx.messages:
% if message.cycle_time is not None:
#define ${message.struct_name.upper()}_CYCLE_TIME_MS (${message.cycle_time}u)
% endif
% endfor

/* Signal choices. */
% for message in ctx.messages:
% for signal in message.signals:
% if signal.choices is not None:
% for value, name in sorted(signal.unique_choices.items()):
% if signal.is_signed:
#define ${message.struct_name.upper()}_${signal.name.upper()}_${name}_CHOICE (${value})
% else:
#define ${message.struct_name.upper()}_${signal.name.upper()}_${name}_CHOICE (${value}u)
% endif
% endfor
% endif
% endfor
% endfor

% for message in ctx.messages:
/**
 * Signals in message ${message.name}.
 *
% if message.comment:
${ctx.indent_c_comment(message.comment)}
 *
% endif
 * All signal values are as on the CAN bus.
 */
struct ${message.struct_name}_t {
% for signal in message.signals:
    /**
% if signal.comment:
${ctx.indent_c_comment(signal.comment, 4)}
     *
% endif
     * Range: ${signal.range_str()}
     * Scale: ${signal.scale_str()}
     * Offset: ${signal.offset_str()}
     */
% if signal.is_float or not ctx.bit_fields:
    ${signal.type_name} ${signal.snake_name};
% else:
    ${signal.type_name} ${signal.snake_name} : ${signal.length};
% endif
% endfor
% if len(message.signals) == 0:
    /**
     * Dummy signal in empty message.
     */
    uint8_t dummy;
% endif
};

% endfor

% for message in ctx.messages:
/**
 * Pack message ${message.name}.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int ${message.struct_name}_pack(
    uint8_t *dst_p,
    const struct ${message.struct_name}_t *src_p,
    size_t size);

/**
 * Unpack message ${message.name}.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int ${message.struct_name}_unpack(
    struct ${message.struct_name}_t *dst_p,
    const uint8_t *src_p,
    size_t size);

% for signal in message.signals:
% if ctx.floating_point_numbers:
/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
${signal.type_name} ${message.struct_name}_${signal.snake_name}_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double ${message.struct_name}_${signal.snake_name}_decode(${signal.type_name} value);

% endif
/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool ${message.struct_name}_${signal.snake_name}_is_in_range(${signal.type_name} value);

% endfor

% endfor
#endif
