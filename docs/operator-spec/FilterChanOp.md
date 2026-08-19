# Operator Specification: FilterChanOp

**Family:** `ChanOp` (Channel Operator)  
**Name:** `Filter`  
**Description:** Applies temporal filtering, smoothing, lag, spring dynamics, and damping to incoming channel streams.

---

## Inputs & Outputs

### Input Pins
- `input` (`PinType::Chan` / `nf::ChannelBuffer`): Source channel stream.
- `reset` (`PinType::Chan`, Optional): Resets internal filter states to current input.

### Output Pins
- `output` (`PinType::Chan` / `nf::ChannelBuffer`): Filtered channel stream.

---

## Parameters

| Name | Type | Default | Description |
|---|---|---|---|
| `type` | `int32_t` (Menu) | 0 | Filter type: `0: Lag (Exponential)`, `1: Spring (2nd order)`, `2: Box (Moving Average)` |
| `lag_up` | `float` | 0.2 | Lag time constant when signal increases (seconds) |
| `lag_down` | `float` | 0.2 | Lag time constant when signal decreases (seconds) |
| `frequency` | `float` | 2.0 | Natural frequency for Spring filter (Hz) |
| `damping` | `float` | 0.707 | Damping ratio for Spring filter ($<1$ underdamped, $1$ critically damped) |
| `filter_width` | `int32_t` | 5 | Window size for Moving Average |

---

## Cook Behavior
1. Reads input channels from `input` pin.
2. Updates persistent per-channel filter states ($y[n-1]$, velocity $v[n-1]$) based on elapsed delta time $\Delta t$.
3. For Lag: $\alpha = 1 - e^{-\Delta t / \tau}$, $y[n] = y[n-1] + \alpha (x[n] - y[n-1])$.
4. For Spring: 2nd-order ODE integration $\ddot{y} + 2\zeta\omega_n\dot{y} + \omega_n^2(y - x) = 0$.
5. Outputs smoothed channel buffer.
