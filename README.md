# BGMi Key Server

Key generation and verification server for BGMi ESP system.

## Features

- Generate time-limited keys (25 hours)
- Track connected devices per key
- Automatic key expiration
- Heartbeat system to keep sessions alive
- Admin panel to manage keys

## API Endpoints

### POST /api/generate
Generate a new key
```bash
curl -X POST https://bgmiserver-tev5.onrender.com/api/generate
```

### POST /api/verify
Verify a key
```bash
curl -X POST https://bgmiserver-tev5.onrender.com/api/verify \
  -H "Content-Type: application/json" \
  -d '{"key":"BGM-XXXX-XXXX-XXXX-XXXX","deviceId":"unique_device_id"}'
```

### POST /api/heartbeat
Send heartbeat to keep session alive
```bash
curl -X POST https://bgmiserver-tev5.onrender.com/api/heartbeat \
  -H "Content-Type: application/json" \
  -d '{"key":"BGM-XXXX-XXXX-XXXX-XXXX","deviceId":"unique_device_id"}'
```

### GET /api/keys
Get all keys (admin panel)
```bash
curl https://bgmiserver-tev5.onrender.com/api/keys
```

### POST /api/disable
Disable a key
```bash
curl -X POST https://bgmiserver-tev5.onrender.com/api/disable \
  -H "Content-Type: application/json" \
  -d '{"key":"BGM-XXXX-XXXX-XXXX-XXXX"}'
```

### POST /api/delete
Delete a key
```bash
curl -X POST https://bgmiserver-tev5.onrender.com/api/delete \
  -H "Content-Type: application/json" \
  -d '{"key":"BGM-XXXX-XXXX-XXXX-XXXX"}'
```

## Admin Panel

Visit the root URL to access the admin panel:
```
https://bgmiserver-tev5.onrender.com
```

## Deployment

The server is deployed on Render at:
```
https://bgmiserver-tev5.onrender.com
```

## Lua Integration

The Lua file includes key verification that:
1. Checks key validity before enabling ESP
2. Sends heartbeat every 5 minutes
3. Disables features when key expires

Server URL configured in Lua:
```lua
local SERVER_URL = "https://bgmiserver-tev5.onrender.com"
```
