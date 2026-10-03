# Fireplace Loader — Upgrade Change Report

## Scope and repository inventory
- Existing project retained; no unrelated project scaffolded.
- Repository source inventory: 50 non-directory project files outside `backend/node_modules/`.
- Areas found: website frontend, admin panel, Node/Express backend, Mongoose models/controllers/routes, API tests, Android Kotlin/WebView app and bundled web assets, environment example, readiness notes, and one-time test report.
- The uploaded archive also contains `backend/node_modules/`; it is intentionally excluded from the deliverable ZIP so dependencies are installed reproducibly from `package-lock.json`.

## Changes made in this pass
1. `backend/src/controllers/adminController.js`
   - Bounded admin user search input to 100 characters.
   - Escaped regex metacharacters in user search so search text is treated literally rather than as an arbitrary regular expression.
   - Removed raw exception-message fields from admin API error responses.
2. `backend/src/controllers/licenseController.js`
   - Added a server-side ownership check: a license assigned to one account cannot be activated/transferred by another account just by knowing the key.
3. `backend/src/models/License.js`
   - Corrected activation validation so a device already registered on a license can reconnect when the activation limit is full.
   - Explicitly rejects revoked, suspended, and expired licenses in the model validity check.
4. `android-app/app/src/main/java/com/fireplace/loader/MainActivity.kt`
   - Always loads the locally bundled application UI. Network unavailability no longer replaces the app with a separate offline page; API/network states can be handled by the app itself.
5. `PRODUCTION_READINESS.md`
   - Updated with the scope and verification limits for this delivery.

## Preserved functionality
- Existing frontend, admin panel, API routes, Mongoose models, test suite, Android project, app branding, and ESP/Visualizer, Location, Health Line, and Aim Lock demonstration screens were retained.
- These game-related screens remain companion-app demonstrations. They do not read live BGMI player state, track other players, access game memory, automate aiming, inject into the game, or bypass anti-cheat.

## Verification
- JavaScript syntax check: PASS — 23 JavaScript files checked with `node --check`, zero syntax failures.
- Backend Jest suite: NOT RUN — `npm test -- --runInBand` was attempted, but the packaged `node_modules/.bin/jest` executable was absent; npm returned `jest: Permission denied`. Install dependencies using `npm ci` in `backend/`, then rerun the suite against a configured test database.
- Android Gradle build: NOT RUN — no system Gradle executable is installed, and the archive does not include the Gradle wrapper JAR/executable. Open `android-app/` in Android Studio with a compatible Gradle installation and build `assembleDebug`; build release only after setting `API_BASE_URL` and signing configuration.
- MongoDB integration / end-to-end / emulator tests: NOT RUN — no configured disposable MongoDB test database, deployed API, or emulator/device was available in this environment.
- Therefore this deliverable is **not certified production-ready**. External configuration and unrun tests remain acceptance blockers.

## Required before production
1. In `backend/`, run `npm ci` and `npm test -- --runInBand` with a disposable test database configured.
2. Configure Atlas least-privilege credentials, network access, backups, and a separate test database.
3. Set production secrets in the host secret manager; use unique, strong `JWT_SECRET` and `JWT_REFRESH_SECRET`, `NODE_ENV=production`, exact `CORS_ORIGIN`, and `REGISTRATION_ENABLED` as intended.
4. Deploy backend and website/admin over HTTPS; configure Android `API_BASE_URL` to the API origin without `/api`.
5. Build and sign the Android release in Android Studio and test on a physical device.
6. Review dependency audit results and finish the remaining checks documented in `PRODUCTION_READINESS.md`.

No APK, deployment URL, database connection, or successful integration/build test is claimed by this report.
