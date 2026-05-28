# BluFizz
Blufizz is a linux command-line tool that continuously scans for nearby Bluetooth (BR/EDR and BLE) and wifi devices. It stores them in a local SQLite database, together with optional GPS coordinates and time statistics.

---

## Features
- Bluetooth BR/EDR and LE passive scan via the BlueZ stack
- Persistent storage of device profiles (address, name, RSSI, TX power, UUIDs, manufacturer data, GATT services, …)
- Time-stamped observations per device
- Optional GPS geo-tagging of each observation via `gpsd`

---

## Architecture overview

```
main
 ├── BluezAdapterManager     – manages BlueZ adapters over D-Bus
 │    └── ObjectManager      – listens for InterfacesAdded / InterfacesRemoved
 ├── BluezDeviceConnector    – connect / disconnect / fetch device profile
 ├── TaskManager             – FIFO async worker (AddDeviceTask …)
 │    ├── DevicesManager     – in-memory registry of seen devices
 │    └── Database           – SQLite persistence (+ GpsdClient for location)
 └── BluFizz                 – main scan loop
```

See [`blufizz_class_diagram.plantuml`](doc/class_diagram.plantuml) for the full PlantUML class diagram.

---

##  Database

BluFizz stores data in SQLite. The schema currently includes:

* radio_devices: generic radio-device identity records.
* bluetooth_details: latest Bluetooth-specific details for each known device.
* radio_observations: timestamped observations, RSSI values, GPS status, and optional GPS coordinates.

---

## Requirements

| Dependency | Purpose |
|---|---|
| C++23 compiler (GCC 13+ / Clang 17+) | Language standard |
| CMake ≥ 3.24 | Build system |
| pkg-config | Build system |
| [sdbus-c++](https://github.com/Kistler-Group/sdbus-cpp) | D-Bus / BlueZ communication |
| [libgps / gpsd](https://gpsd.gitlab.io/gpsd/) | GPS coordinates |
| [nlohmann/json](https://github.com/nlohmann/json) | JSON serialisation of device properties |
| [SQLite3](https://www.sqlite.org/) | Local device database |
| BlueZ (Linux) | Bluetooth stack |

---

## Build

```bash
# Install dependencies (Debian / Ubuntu)
sudo apt install cmake pkg-config libsdbuscpp-dev libgps-dev nlohmann-json3-dev libsqlite3-dev bluez gpsd gpsd-clients

# Configure and build
git clone https://github.com/FranckTheMotte/blufizz.git
cd blufizz
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

The resulting binary is `build/blufizz`.

---

## Usage

BluFizz requires access to the system D-Bus and the Bluetooth adapter. Ensure your user is in the `bluetooth` group:

```bash
./build/blufizz
```

The tool will:

1. Power on the default Bluetooth adapter (`/org/bluez/hci0`).
2. Start continuous BR/EDR + LE discovery.
3. Connect to each discovered device, fetch its full profile, and persist it to `/tmp/blue.sqlite3`.
4. Attach GPS coordinates to each observation if `gpsd` is running.
5. On `Ctrl-C`, cleanly disconnect all connected devices and exit.

The SQLite database is created at `/tmp/blue.sqlite3` on first run.

---

## Limitations

- Devices advertising with a **random address** may be recorded multiple times across sessions, as no stable identifier is available.
- Only the default adapter (`hci0`) is currently used.
- Wi-Fi device scanning is not yet implemented.

---

## Roadmap

Planned or incomplete:

- [ ] Handle update event of bluetooth devices.
- [ ] Wi-Fi device scanning
- [ ] GPS geo-tagging (in progress — GpsdClient exists, integration pending)
- [ ] Exception handling throughout the codebase (plan remove exceptions (compiler options))
- [ ] Limit on the number of persistent observations
- [ ] Limit on the number of devices recorded per session
- [ ] Input sanity checks
- [ ] Configuration file (adapter path, database path, …)
- [ ] Tests
- [ ] Add capability to start, pause or stop a session from an external device (smartphone, ...)
- [ ] Record gps trace of a session.

---

## License

MIT — see [LICENSE](LICENSE) for details.

---

## Authors

See [AUTHORS](AUTHORS).
