# Fireplace Loader — Safe Device & BGMI Compatibility Test Guide

This guide tests whether the companion app launches and its independent features work. It does not test or enable BGMI game-memory access, live enemy tracking, injection, anti-cheat bypass, or automated aiming.

## Before testing

- Use a test account and a non-production license where possible.
- Configure the deployed HTTPS API origin in the Android build (`API_BASE_URL`).
- Do not put MongoDB credentials, JWT secrets, or keystore passwords in frontend files.
- Install the debug APK on a physical Android device and note the device model, Android version, app version, and build variant.

## App smoke tests

1. Launch Fireplace Loader while online. Confirm the UI loads and there is no crash.
2. Turn on Airplane mode and reopen the app. Confirm the bundled UI still loads and network-dependent actions show a useful offline/error state rather than a false success.
3. Restore internet. Register/login with a test account, sign out, and sign in again.
4. Try a wrong password and an expired/revoked test license. Confirm clear errors and no unauthorized access.
5. Activate a valid test license. Confirm the status and expiry shown by the app match the API response.
6. Restart the app and confirm session/settings behavior is consistent with the implementation.
7. Test an ordinary user against admin-only screens/API routes; access must be denied server-side.

## Visualizer module tests

1. Open the ESP/Visualizer demo and start the sample scene. Confirm labels/boxes appear for clearly simulated entities.
2. Change entity count/speed, pause, resume, reset, and stop. Confirm controls respond and no animation continues after stop/navigation.
3. Change visualizer colours, opacity, labels, and distance settings. Navigate away and return/restart; confirm settings persist where supported.
4. Toggle Health Line and verify bars respond to the local sample health values, including low and full health.
5. Toggle the Aim Lock demo and confirm it only draws a crosshair/target-radius/nearest sample target inside the demo canvas. It must not control BGMI input or alter BGMI aim.
6. Verify any entity names/coordinates are visibly labelled as simulated/test data.

## Location feature

The current app manifest does not request device location permission. Do not treat the visualizer's sample markers as real GPS. If native location is added later, test permission grant, denial, approximate location, location unavailable, timestamp/accuracy display, and permission revocation. Only the device's own location should be accessed with explicit consent.

## BGMI launch compatibility (non-invasive)

1. Launch BGMI normally from its own app icon, without injecting, overlaying, attaching a debugger, reading its memory, or automating input.
2. Switch between BGMI and Fireplace Loader using Android Recents; confirm neither app crashes during normal background/foreground transitions.
3. Check Android Settings > Apps for crashes or ANRs after the test. Record device model, Android version, timestamps, and reproducible steps.
4. This verifies only normal coexistence/launch compatibility. It does not prove that ESP, live player tracking, game health reading, or automated aiming works in BGMI; those functions are not implemented.

## Record results

For each case, record `PASS`, `FAIL`, or `BLOCKED`, plus steps and a redacted log. Never include passwords, tokens, license keys, or personal data in logs/screenshots.

## Current environment status

- JavaScript syntax checks: PASS.
- Website-to-Android bundled frontend sync: PASS.
- Android manifest network permissions: PASS.
- Jest suite: BLOCKED (`jest` executable not found in the current environment).
- APK build/device tests: BLOCKED (Gradle wrapper executable/JAR and device/emulator unavailable here).
- Live API/MongoDB checks: NOT RUN (no deployed API URL or test database credentials configured).
