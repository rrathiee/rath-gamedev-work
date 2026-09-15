# rath-gg

A lightweight DayZ quality-of-life mod that warns a player when equipped gear becomes **Badly Damaged** or **Ruined**.

## Why I Built It

DayZ already exposes equipment condition, but during longer sessions it is easy to miss that an important item has deteriorated. `rath-gg` reduces inventory-checking busywork without changing item health, repairing gear, or exposing hidden player stats.

## Features

- Periodically checks worn equipment.
- Also checks the item currently in the player's hands.
- Warns only at **Badly Damaged** or **Ruined**.
- Tracks the last observed item/state for each slot.
- Does not repeat the same warning every check.
- Notices when an item is replaced or repaired.
- Does not modify inventory contents or item health.


## Possible Version 2

- JSON configuration
- Per-slot enable/disable settings
- Different thresholds for weapons and clothing
- Server-enforced settings
- Optional repair-kit reminder
- Custom icon/UI preferences
