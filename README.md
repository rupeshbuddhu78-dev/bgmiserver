# BGMi Key Server

Key generation and verification server for the BGMi ESP system.

## Features

- Generate time-limited keys (default 25 hours)
- Configure the maximum devices allowed per key (for example 100 or 500)
- Persistent key, device-session and configuration storage in `data/*.json`
- Heartbeat system to keep active sessions alive
- Admin panel with login session retained in the browser for 30 days
- Admin panel to generate, enable, disable and delete keys

## Device limit configuration

The default limit is `1`. It can be changed in either of these ways:

1. **Admin panel:** log in, enter the desired number in **Max devices per key**, and click **Save Limit**.
2. **Environment variable:** set `MAX_DEVICES_PER_KEY=500` before starting the server. This takes precedence over `data/config.json`.

The admin panel stores the signed login token in browser `localStorage`, so a page refresh or server restart does not require login again. Clicking **Logout** removes it.

## Persistent data

The server automatically creates this directory on startup:

```text
data/
  config.json   # current maxDevicesPerKey
  keys.json     # generated keys and their registered device IDs
  devices.json  # device sessions and last heartbeat timestamps
```

These runtime JSON files are intentionally ignored by Git so secrets and live customer data are not committed. The `data/.gitkeep` file keeps the directory in the repository.

## API endpoints

### POST `/api/generate` (admin)

```bash
curl -X POST http://localhost:3000/api/generate \
  -H "Content-Type: application/json" \
  -H "x-admin-token: ADMIN_TOKEN" \
  -d '{"hours":25,"customKey":"BGM-TEST"}'
```

### POST `/api/verify` or GET `/api/verify` (client)

```bash
curl -X POST http://localhost:3000/api/verify \
  -H "Content-Type: application/json" \
  -d '{"key":"BGM-TEST","deviceId":"unique_device_id"}'
```

### POST `/api/heartbeat` (client)

```bash
curl -X POST http://localhost:3000/api/heartbeat \
  -H "Content-Type: application/json" \
  -d '{"key":"BGM-TEST","deviceId":"unique_device_id"}'
```

### POST `/api/admin/login`

Send `{ "password": "..." }` to receive a signed admin token. The default password is only for development; set `ADMIN_PASSWORD` in production.

### GET/POST `/api/settings` (admin)

Read or update the global device limit:

```bash
curl -X POST http://localhost:3000/api/settings \
  -H "Content-Type: application/json" \
  -H "x-admin-token: ADMIN_TOKEN" \
  -d '{"maxDevicesPerKey":500}'
```

Other admin endpoints are `/api/keys`, `/api/disable`, `/api/enable` and `/api/delete`.

## Run locally

```bash
npm install
ADMIN_PASSWORD='change-this-password' npm start
```

## Lua integration

The Lua file includes key verification and heartbeat calls. Update its `SERVER_URL` to the deployed server URL when deploying.
