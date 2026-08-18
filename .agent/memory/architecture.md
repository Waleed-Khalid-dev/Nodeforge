---
type: project
created: 2026-07-20
updated: 2026-07-25
---

# Crypto Dashboard / NexusDesk Architecture

## Tech Stack
- **Framework**: Electron (Main + Renderer processes with WebContentsView multi-pane layout)
- **APIs**: CCXT (Cross-exchange Spot/Futures), Google Gemini AI, CoinMarketCap, LunarCrush
- **UI**: Vanilla HTML5/CSS3 (Dark-mode Glassmorphism), Chart.js
- **Security**: Windows DPAPI (`electron.safeStorage`) for encrypted key vault in `%AppData%\NexusDesk\`

## Key Design Patterns & Milestones
- **Multi-Pane WebContentsView Layout**: Top control bar, CryptoBubbles, TradingView chart, Coinglass Heatmap, and dynamic bottom panel (CMC / AI Co-Pilot / Vault / Arbitrage).
- **Navigation Fix**: Overridden `will-prevent-unload` events on TradingView `WebContents` to prevent chart interactions from blocking coin search navigation.
- **Graphify & Serena Memory**: Indexed 444 nodes / 675 edges knowledge graph (`graphify-out/`) and `.serena/memories/` cognitive brain.
- **Synchronized Repositories**: `crypto-dashboard` (dev) and `NexusDesk` (production public repo at `Waleed-Khalid-dev/NexusDesk`).
- **Multi-Tab Workspace System**: Glassmorphism tab bar in control bar allowing users to open, switch, rename, and close coin monitoring tabs. Persists tab states across app restarts via `%AppData%\NexusDesk\hub-settings.json` (`workspaceTabs`, `activeTabId`). Synchronizes cross-pane layouts on tab switch.
- **Documentation**: Standardized `docs/ARCHITECTURE.md`, `llms.txt`, and clean UTF-8 `README.md`.


