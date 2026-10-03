# Production Readiness / Deployment Checklist

## Fixed in this revision
- Corrected API base URL handling: it must be the server origin, not an origin ending in `/api`; this fixes duplicated `/api/api/...` calls.
- Removed wildcard CORS with credentials. Browser origins are checked against the comma-separated `CORS_ORIGIN` allowlist; native requests without an Origin header are supported.
- Added startup validation for required MongoDB/JWT configuration and production secret strength / CORS configuration.
- Enforced `REGISTRATION_ENABLED=false` on the registration endpoint (not only in the public config response).
- Added explicit refresh-token type validation and stopped returning raw exception messages from selected auth/license endpoints.
- Documented API origin configuration for Android debug and release builds.
- Kept Android permissions minimal: Internet and network state only.

## Required before a real production launch
- [ ] Create a MongoDB Atlas database user with least privilege; configure network access for the actual server and use a strong URI password.
- [ ] Set `MONGODB_URI`, `JWT_SECRET`, `JWT_REFRESH_SECRET` (two different random secrets), `NODE_ENV=production`, `CORS_ORIGIN`, `APP_VERSION`, and any hosting-specific variables in the hosting provider's secret manager.
- [ ] Set `CORS_ORIGIN` to exact website origins, comma-separated; never use `*` in production.
- [ ] Deploy the backend behind HTTPS and configure `TRUST_PROXY` only if the hosting topology requires it.
- [ ] Deploy frontend and admin panel on HTTPS. Configure the admin panel's API URL to the backend origin.
- [ ] Build Android release with `API_BASE_URL` set to the backend origin (no `/api` suffix), then sign with your own protected keystore.
- [ ] Create a least-privilege admin account through a controlled server-side process; do not make public registration grant admin role.
- [ ] Run the automated API test suite against a disposable test database; perform end-to-end testing on a real Android device and current browsers.
- [ ] Configure backups, log retention, monitoring, dependency vulnerability review, privacy notice, terms, support/contact details, and incident response.
- [ ] Verify license activation concurrency and add atomic database operations before relying on activation limits for paid subscriptions.
- [ ] Consider hashing refresh tokens at rest and adding email verification/password reset before public launch.

## Not included
This project does not implement real BGMI player tracking, game process memory access, game-integrated overlays, aim-lock/aimbot, injection, or anti-cheat bypass. The ESP/location/health/aim screen remains a self-contained visualizer and must not be represented as a working game feature.

## Build notes
The ZIP does not contain a compiled APK or Gradle wrapper executable/JAR. Open `android-app/` in Android Studio and sync Gradle, or generate a wrapper using an installed compatible Gradle distribution. Then build `assembleDebug` for testing or `assembleRelease -PAPI_BASE_URL=https://api.your-domain.tld` for a configured release. A release APK still requires signing and real-device testing.


## Verification status for this delivered source
See `CHANGE_REPORT.md` for exact changes and test results. JavaScript syntax checks passed. The Jest command could not start because the bundled `.bin/jest` executable was missing/not executable in this archive. Android compilation and MongoDB-backed integration tests were not run in this environment. Do not treat this ZIP as production-certified until those checks pass.
