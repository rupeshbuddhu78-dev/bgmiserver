# Education License Demo

A standalone educational Node.js license-key server for a BCA project. It is intentionally independent of game code, process injection, memory modification, and anti-cheat systems.

## Run locally

```bash
npm install
cp .env.example .env
# edit ADMIN_TOKEN in .env
npm start
```

Open `http://localhost:3000/` for the client demo and `http://localhost:3000/admin.html` for the admin panel.

## API

- `GET /api/health`
- `POST /api/validate-key` with `{ "key": "...", "appId": "education-demo", "deviceId": "demo-browser" }`
- Admin endpoints require `Authorization: Bearer $ADMIN_TOKEN`:
  - `GET /api/admin/keys`
  - `POST /api/admin/keys` with `{ "days": 30, "appId": "education-demo" }`
  - `PATCH /api/admin/keys/:id`
  - `DELETE /api/admin/keys/:id`

## Deploy on Render

1. Create a new **Web Service** and connect this repository.
2. Build command: `npm install`
3. Start command: `npm start`
4. Add environment variables `ADMIN_TOKEN`, `APP_ID`, and `CORS_ORIGIN`.
5. For persistence, attach a Render Disk mounted at `/var/data` and set `DATA_FILE=/var/data/licenses.json`.
6. Use the generated `https://...onrender.com` URL as the API base URL for your legitimate demo client.

The included JSON storage is suitable for a classroom demonstration. For production, use a managed database and rotate admin credentials.
