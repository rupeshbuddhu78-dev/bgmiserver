# Fireplace Loader - Source Change Report
## Version 4.6.0 -> 4.7.0

---

## File-by-File Changes

### 1. `android-app/app/src/main/AndroidManifest.xml`
**Change Type:** Modified
**Changes:**
- Added `ACCESS_FINE_LOCATION` permission
- Added `ACCESS_COARSE_LOCATION` permission
**Reason:** Required for real device location feature.

---

### 2. `android-app/app/src/main/java/com/fireplace/loader/MainActivity.kt`
**Change Type:** Major Rewrite
**Changes:**
- Added `LocationManager` integration with GPS and Network providers
- Added `LocationListener` for real-time location updates
- Added `hasLocationPermission()` - checks runtime permission status
- Added `requestLocationPermission()` - triggers Android permission dialog
- Added `startLocationUpdates()` / `stopLocationUpdates()` - manage location polling
- Added `getLastLocation()` - returns last known device location
- Added `isLocationEnabled()` - checks if device location services are on
- Added `onRequestPermissionsResult()` - handles permission grant/deny callback
- Added `locationToJson()` - serializes Location to JSON for WebView bridge
- WebView `evaluateJavascript` callback: `onNativeLocationUpdate(json)` pushes live location to JS
- WebView `evaluateJavascript` callback: `onLocationPermissionResult(granted)` notifies JS of permission outcome
- **AppBridge** new methods:
  - `hasLocationPermission(): Boolean`
  - `requestLocationPermission()`
  - `isLocationEnabled(): Boolean`
  - `startLocationUpdates()`
  - `stopLocationUpdates()`
  - `getLastLocation(): String` (JSON)
- Location updates stopped in `onDestroy()` to prevent leaks
**Reason:** Replaces simulated coordinates with actual Android location APIs.

---

### 3. `android-app/app/build.gradle`
**Change Type:** Modified
**Changes:**
- Default API URL changed from `https://api.example.com` to `https://bgmiserver-tev5.onrender.com`
**Reason:** Connect app to the user's actual backend server.

---

### 4. `android-app/app/src/main/assets/www/js/api.js`
**Change Type:** Modified
**Changes:**
- Default `baseUrl` fallback changed from `http://localhost:3001` to `https://bgmiserver-tev5.onrender.com`
**Reason:** Match the production server URL.

---

### 5. `android-app/app/src/main/assets/www/js/demo-scene.js`
**Change Type:** Complete Rewrite
**Changes:**

#### ESP / Visualizer
- Removed: `Math.random()` for entity positions, names, health, colors, sizes
- Added: `_buildTestDataset()` - 10 explicitly defined test entities with fixed properties
- Added: Grid-based deterministic placement (no random positions)
- All entities flagged with `isTestEntity: true` and `[TEST]` prefix in names
- Added: `supplyEntityData()` method for future authorized data source integration
- Added: "No entity data available" state when no entities exist
- Watermark changed to: "DEMO SCENE - Test Data Only - No Live Game Data"

#### Location
- Removed: Random coordinate simulation
- Added: `initLocation()` - checks Android bridge, permission, location services
- Added: `onNativeLocationUpdate()` - receives real GPS data from native layer
- Added: `onLocationPermissionResult()` - handles permission grant/deny
- Added: Location states: `idle | requesting | granted | denied | disabled | error`
- Added: `updateLocationPanel()` - updates HTML panel with real lat/lng/accuracy/time
- Location markers and minimap only rendered when `locationState === 'granted'`

#### Health Line
- Removed: `e.health += (Math.random() - 0.505) * 0.5` random health fluctuation
- Health values now come exclusively from the test dataset's `baseHealth` field
- Added: `setEntityHealth(entityId, value)` - explicit health value setter (only way to change health)
- Health bars render actual values, no random drift

#### Aim-Lock
- Removed: Fake aim-lock targeting closest random entity
- Added: Independent aim-training tool
- Added: `initAimTraining()` - sets up score, hits, misses tracking
- Added: `_spawnAimTarget()` - deterministic target placement using golden ratio
- Added: `_setupAimClickHandler()` - canvas click detection for hitting targets
- Added: `updateAimTraining()` - target lifetime, expiry, auto-spawn logic
- Added: `_drawAimTraining()` - renders targets with timer arcs, crosshair, score HUD
- Configurable: crosshair size, color, opacity, target radius, auto-spawn
- Settings persisted via `localStorage`

#### Global Callbacks
- Added: `onNativeLocationUpdate(data)` - global function for Android bridge
- Added: `onLocationPermissionResult(granted)` - global function for Android bridge

---

### 6. `android-app/app/src/main/assets/www/js/app.js`
**Change Type:** Modified
**Changes:**
- `demoScene` exposed as `window.demoScene` for native callbacks
- Location toggle handler: checks `isLocationEnabled()` and `hasLocationPermission()` before enabling
- Feature validation: requires at least one feature enabled before launching demo
- Location panel show/hide synced with location feature toggle
- `getDeviceId()` now prefers native `AndroidBridge.getDeviceId()` when available
- Settings UI: added loading/saving for new aim settings (crosshair size, opacity, target radius, auto-spawn)
- `gatherSettings()` includes all new aim-trainer settings
- Server status shows "Checking..." state before result arrives (no false "Connected" flash)

---

### 7. `android-app/app/src/main/assets/www/index.html`
**Change Type:** Modified
**Changes:**
- Version bumped to v4.7.0 / Build 2024.2
- Feature descriptions updated:
  - ESP: "Data-driven bounding boxes & labels" (was "Bounding boxes, labels, distance")
  - Location: "Real GPS location with coordinates" (was "Simulated markers & minimap")
  - Health: "Controlled health bar display" (was "Health bars & indicators")
  - Aim-Lock: "Configurable crosshair & targets" (was "Target visualization & crosshair")
- Feature titles updated: "ESP Visualizer", "Device Location", "Health Line", "Aim Trainer"
- Added: Location Info Panel (`#location-info-panel`) with lat/lng/accuracy/time fields
- Added: New settings controls:
  - Crosshair Size (range 6-30)
  - Crosshair Opacity (range 10-100)
  - Target Size (range 10-50)
  - Auto-Spawn Targets (checkbox)

---

### 8. `android-app/app/src/main/assets/www/css/style.css`
**Change Type:** Modified
**Changes:**
- Added: `.location-info-panel` styles (background, padding, border)
- Added: `.loc-panel-header`, `.loc-panel-title` styles
- Added: `.loc-status`, `.loc-status-active`, `.loc-status-error` badge styles
- Added: `.loc-panel-grid` (2-column grid layout)
- Added: `.loc-item`, `.loc-label`, `.loc-value` styles

---

### 9. Frontend Mirror Files (5 files)
All files under `frontend/` synced to match their `android-app/` counterparts:
- `frontend/js/app.js`
- `frontend/js/demo-scene.js`
- `frontend/js/api.js`
- `frontend/index.html`
- `frontend/css/style.css`

---

## Summary of Removed Simulated/Fake Code

| Feature | Removed | Replaced With |
|---------|---------|---------------|
| ESP | `Math.random()` positions, hardcoded name array, random health/colors | Explicit test dataset, grid placement, `[TEST]` labels |
| Location | Random canvas coordinates | Android LocationManager (GPS + Network), permission handling |
| Health | `e.health += (Math.random() - 0.505) * 0.5` | Fixed dataset values, explicit `setEntityHealth()` API |
| Aim-Lock | Fake crosshair on closest random entity | Independent aim trainer with clickable targets, scoring |

## False States Removed
- No more fake "LIVE" indicators
- No more fabricated player names or positions
- Server status shows "Checking..." before result (no false "Connected")
- Location shows proper states: Waiting, Requesting, Active, Denied, Disabled, Error
