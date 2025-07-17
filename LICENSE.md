# CUSTOM SOFTWARE & HARDWARE LICENSE AGREEMENT  
Copyright (c) 2025 [RestlessByte - https://github.com/RestlessByte]  
All Rights Reserved Worldwide

---

## 1. Definitions

- **"Software"**: Entire repository content including all:
  - Arduino source code (C/C++, `.ino`, `.h`, `.cpp`)
  - Firmware logic, algorithms, timing sequences, and pin mappings
  - Configuration files, calibration data, and documentation

- **"Hardware Design"**: All physical design elements:
  - Circuit diagrams, wiring layouts, PCB files (Gerber, KiCad, Fritzing)
  - Electrical component choices, part lists (BOM)
  - Mechanical schematics related to the project

- **"Template Use"**: Any attempt to utilize this Project as:
  - A starting point for new Arduino/electronics projects
  - A structural reference for re-implementing code or hardware
  - Basis for derivative work (modified clones, forks with changes)

- **"End-Product"**: A final embedded system/device where this project is integrated (excluding devkits or tutorial clones)

---

## 2. License Grant & Restrictions

### 🔐 Core Prohibitions (Absolute):
**You are STRICTLY FORBIDDEN to:**
- ❌ Use this project as a boilerplate for new Arduino or electronics builds  
- ❌ Repurpose any logic, wiring schemes, or code sequences  
- ❌ Publish forks with visual/schematic/code similarity  
- ❌ Include parts of this project in kits, starter templates, or YouTube tutorials  
- ❌ Redistribute or sell modified/unmodified versions

---

### 🆓 Free Tier (Non-Commercial Use Only):  
✅ Permitted ONLY for:
- Local testing and experimentation using personal hardware
- Educational examination of logic/code for learning only  
- Offline tinkering with **no public-facing project**

⚠️ **No reuse of schematic, code, pin layout, or structure is allowed — even partially**

---

### 💳 Pro Tier (Commercial / Licensed Use):
**Requires a paid license and written permission.**  
✅ Allowed (with license) for:
- Integration into **custom devices** as long as:
  - Final product doesn’t expose internal design/code
  - PCB/wiring/design is **substantially transformed**
- Use inside private/internal hardware not intended for resale or sharing

---

## 3. Design Protection Clause

>The unique combination of firmware logic, wiring structure, component layout, and project architecture represents original intellectual and creative work.  
>Any reproduction or adaptation of these elements—fully or in part—into derivative projects without explicit written approval and license is strictly forbidden.

---

## 4. Violation Penalties

- 🚫 Immediate and automatic termination of all rights  
- 💰 Liability for legal/statutory damages (up to **$150,000 USD under DMCA**)  
- ⛔ Obligation to permanently destroy derivative works within **72 hours of notice**

---

## 5. Enforcement Example

**Prohibited Reuse:**

```cpp
// Copied initialization logic
void setup() {
  pinMode(9, OUTPUT); // same pin, same logic
  analogWrite(9, 255); // copied behavior
}