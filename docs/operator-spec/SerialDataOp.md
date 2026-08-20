# SerialDataOp

## Overview
`SerialDataOp` establishes high-throughput asynchronous communication with serial devices (USB microcontrollers, Arduino, ESP32, RS-232/RS-485), parsing incoming sensor lines into a `DataTable` and transmitting command strings.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::SerialDataOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `port` | `String` | `"COM3"` | - | - | Serial COM port name (e.g. `COM1`, `COM3`, `/dev/ttyUSB0`). |
| `baud_rate` | `Int` | `115200` | 1200 | 921600 | Serial communication baud rate. |
| `data_bits` | `Int` | `8` | 5 | 8 | Data bits. |
| `stop_bits` | `Int` | `1` | 1 | 2 | Stop bits (1 or 2). |
| `parity` | `Int` | `0` | 0 | 2 | Parity: 0=None, 1=Odd, 2=Even. |
| `delimiter` | `String` | `"\n"` | - | - | Message delimiter string. |
| `max_rows` | `Int` | `100` | 1 | 10000 | Maximum history rows in output `DataTable`. |
| `active` | `Bool` | `true` | - | - | Enable/disable serial worker thread. |

## Inputs / Outputs
- **Inputs:**
  - `send_data` (`PinType::Data`, optional): Incoming table of strings or pulses to write to serial port.
- **Outputs:**
  - `output` (`PinType::Data`): Received lines log table (`timestamp`, `message`, `value0`, `value1`...).
