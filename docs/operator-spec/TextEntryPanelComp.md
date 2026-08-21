# Operator Specification: TextEntryPanelComp

**Family:** `Comp`  
**Type Name:** `TextEntryPanelComp`  
**Version:** 1.0  
**Phase Ownership:** Phase 15 (Epic 15.3)  

---

## 1. Description

`TextEntryPanelComp` provides an editable text input field and dynamic value readout display within UI panels.

---

## 2. Pins

### Input Pins
| Pin Name | Type | Required | Description |
|---|---|---|---|
| `in_text` | `Data` | No | Optional input table or string feeding text content |

### Output Pins
| Pin Name | Type | Description |
|---|---|---|
| `output` | `Comp` | UI widget descriptor connected into parent `PanelComp` |
| `out_data` | `Data` | 2D table containing the current text string |

---

## 3. Parameters

| Parameter | Type | Default | Range | Description |
|---|---|---|---|---|
| `label` | `string` | `"Text Input"` | - | Field label |
| `text` | `string` | `""` | - | Current text value |
| `read_only` | `bool` | `false` | `true/false` | Display-only mode |

---

## 4. Cook Semantics

- **Time-Dependent:** No.
