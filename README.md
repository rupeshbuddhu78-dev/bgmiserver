# Fireplace Loader

A BGMI companion application project with a dark-themed website, user authentication, server-side license verification, an admin panel, and a native Android WebView shell. It is not a game modification tool.

## Architecture

```
├── android-app/        # Android Studio project (Kotlin + WebView)
├── backend/            # Node.js/Express REST API
├── frontend/           # HTML/CSS/JS (bundled in Android WebView)
├── admin-panel/        # Admin management interface
└── docs/               # Documentation
```

## Features

- **Authentication**: Register, login, token refresh, logout, rate limiting
- **License System**: Key generation, activation, expiration, device binding
- **Training visualizer**: Self-contained 2D simulated entities for UI testing only (not real game tracking or aim control)
- **Admin Panel**: User management, license management, audit logs, statistics
- **Security**: JWT auth, bcrypt passwords, rate limiting, CORS, Helmet

## Important Disclaimer

All demo features (ESP, Location, Health-Line, Aim-Lock) use **simulated entities** within the application's own test environment. They do NOT:
- Read or access BGMI's process memory
- Inject code into the game
- Extract hidden player data
- Modify game behavior
- Bypass anti-cheat protections

## Quick Start

### Prerequisites

- Node.js 18+
- MongoDB Atlas account (or local MongoDB)
- Android Studio (for APK build)

### 1. Backend Setup

```bash
cd backend
cp .env.example .env
# Edit .env with your MongoDB Atlas URI and JWT secrets
npm install
npm start
```

### 2. Frontend Preview

```bash
cd frontend
python3 -m http.server 3000
# Open http://localhost:3000
```

### 3. Admin Panel

```bash
cd admin-panel
python3 -m http.server 3001
# Open http://localhost:3001
```

### 4. Android Build

1. Open `android-app/` in Android Studio
2. Sync Gradle
3. Configure the API origin (without `/api`) using `-PAPI_BASE_URL=https://api.your-domain.tld` or the `API_BASE_URL` environment variable
4. Build > Build Bundle(s)/APK(s) > Build APK
5. The APK will be in `app/build/outputs/apk/debug/`

## API Documentation

### Public Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/health` | Health check |
| GET | `/api/config` | App configuration |
| POST | `/api/auth/register` | Register new user |
| POST | `/api/auth/login` | Login |
| POST | `/api/auth/refresh` | Refresh access token |

### Authenticated Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| POST | `/api/auth/logout` | Logout |
| GET | `/api/auth/me` | Get current user |
| POST | `/api/licenses/activate` | Activate license key |
| GET | `/api/licenses/status` | Get license status |
| POST | `/api/licenses/validate` | Validate active license |

### Admin Endpoints (requires admin role)

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/admin/users` | List users |
| PATCH | `/api/admin/users/:id/status` | Update user status |
| POST | `/api/admin/licenses` | Create license key |
| GET | `/api/admin/licenses` | List licenses |
| PATCH | `/api/admin/licenses/:id/revoke` | Revoke license |
| PATCH | `/api/admin/licenses/:id/suspend` | Suspend license |
| GET | `/api/admin/audit-logs` | View audit logs |
| GET | `/api/admin/stats` | Get statistics |

## MongoDB Atlas Setup

1. Create an account at [mongodb.com/cloud/atlas](https://www.mongodb.com/cloud/atlas)
2. Create a new cluster (free tier M0 is sufficient)
3. Create a database user with read/write permissions
4. Add your server IP to Network Access (or allow 0.0.0.0/0 for development)
5. Get the connection string and update `.env`

## Deployment

### Backend (e.g., Railway, Render, Heroku)

1. Push backend code to a Git repository
2. Connect to your hosting platform
3. Set environment variables from `.env.example`
4. Deploy - the platform provides HTTPS automatically

### Android APK

1. Set `API_BASE_URL` to your deployed backend origin (without `/api`) when building, e.g. `./gradlew assembleRelease -PAPI_BASE_URL=https://api.your-domain.tld`
2. In Android Studio: Build > Generate Signed Bundle/APK
3. Create a new keystore or use existing
4. Select "release" build variant
5. Distribute the signed APK

## Security Notes

- Never commit `.env` files with real secrets
- Use strong, unique JWT secrets in production
- Enable HTTPS for all API communication
- MongoDB credentials are server-side only (never in APK)
- Rate limiting protects against brute-force attacks
- Passwords are hashed with bcrypt (12 rounds)

## Testing

```bash
cd backend
npm test
```

Tests cover:
- Registration validation
- Login flows (success, failure, lockout)
- Token refresh
- Protected route access
- License activation
- Admin access control
- API validation
- 404 handling

## Production readiness notes

- `API_BASE_URL` is an origin only; frontend routes append `/api/...` themselves. Do not include `/api` in the value.
- The source ZIP intentionally does not include MongoDB credentials, JWT secrets, signing keys, or a compiled release APK.
- Before deployment, configure the real Atlas URI, separate random JWT secrets, exact `CORS_ORIGIN` values, HTTPS, hosting health checks, backups, and monitoring.
- The app requests only Internet and network-state permissions. Do not add overlay, accessibility, storage, or other permissions unless a clearly disclosed legitimate feature requires them.
- The visualizer is a local simulation. This project does not implement BGMI process-memory reading, player tracking, game-integrated overlays, aim-lock/aimbot, injection, or anti-cheat bypass. Such game-integrated cheating features are not included. Legitimate device diagnostics, network status, and user-controlled graphics presets can be developed separately.
- The Android WebView loads bundled app assets and blocks external navigation. Keep this restriction; do not load untrusted web pages into the JavaScript-enabled WebView.

## License

This project is for educational and companion-app purposes only.
