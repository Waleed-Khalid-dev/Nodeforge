# WebDataOp

## Overview
`WebDataOp` performs non-blocking asynchronous HTTP REST requests (GET/POST/PUT/DELETE) and populates the response status code, headers, and body text/JSON into output tabular rows without frame drops.

## Category
- **Family:** `DataOp` (DAT)
- **C++ Class:** `nf::WebDataOp`
- **Output Type:** `PinType::Data` (`DataTable`)

## Parameters

| Parameter | Type | Default | Min | Max | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `url` | `String` | `"https://httpbin.org/get"` | - | - | Target HTTP URL endpoint. |
| `method` | `Int` | `0` | 0 | 3 | 0: GET, 1: POST, 2: PUT, 3: DELETE. |
| `body` | `String` | `""` | - | - | Request payload body (JSON/text). |
| `headers` | `String` | `"Content-Type: application/json"` | - | - | Request HTTP headers (newline-separated). |
| `auto_fetch` | `Bool` | `true` | - | - | Fetch automatically on parameter change. |
| `pulse_fetch` | `Bool` | `false` | - | - | Trigger one-shot fetch. |

## Inputs / Outputs
- **Inputs:**
  - `input` (`PinType::Data`, optional): Input table to format as POST body.
- **Outputs:**
  - `output` (`PinType::Data`): Response headers, status, and payload in `DataTable`.
