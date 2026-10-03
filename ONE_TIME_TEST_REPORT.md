# Fireplace Loader — one-time test report

Date: 2026-10-03

## Checks performed once

- PASS — Node.js syntax checks for JavaScript files in the website, admin panel, backend source/tests, and Android-bundled JavaScript.
- PASS — Website frontend files match the corresponding Android WebView-bundled files (`index.html`, stylesheet, API client, app JS, and demo visualizer).
- PASS — Android manifest declares `INTERNET` and `ACCESS_NETWORK_STATE`; no location, contacts, SMS, storage, or accessibility permissions were found in the manifest.
- BLOCKED — Automated Jest API suite did not run: the `jest` executable was unavailable after dependency installation timed out. No test pass is claimed.
- BLOCKED — Android APK was not built: Android SDK/Gradle wrapper executable and wrapper JAR are not present in this environment.
- NOT RUN — Live MongoDB Atlas connection, hosted API, website deployment, and real-device tests. These require the owner's Atlas URI, hosting/domain configuration, and Android build/signing environment. No production credentials were supplied.

## What this means

Static syntax and asset-sync checks passed, but they do not prove that registration, login, licenses, Atlas connectivity, deployment, or the APK works end-to-end. Do not treat this source archive as production-verified until the blocked checks are completed.

## Run the one-time check

From this project folder:

```bash
bash scripts/one-time-check.sh
```

To additionally check a deployed API health endpoint (origin only; do not append `/api`):

```bash
API_BASE_URL=https://your-api-domain.example bash scripts/one-time-check.sh
```

Do not paste database passwords, JWT secrets, or Android keystore passwords into source files or public repositories. Configure them in the hosting provider's secret/environment settings.

## Still needs owner configuration

1. Create/configure MongoDB Atlas database user and network access, then set `MONGODB_URI` in the hosting provider.
2. Set two different strong random JWT secrets, `NODE_ENV=production`, and exact `CORS_ORIGIN` website origins.
3. Deploy backend and frontend/admin over HTTPS.
4. Set `API_BASE_URL` to the deployed backend origin when building Android.
5. Build and sign APK using Android Studio and the owner's protected signing key; test on a real device.

Real BGMI player tracking, game-integrated ESP, and aim-lock are not implemented. This project remains a companion app with a simulated visualizer; the test package does not add game-memory access, injection, or anti-cheat bypass.
