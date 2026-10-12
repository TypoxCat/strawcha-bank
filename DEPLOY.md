# Deploy notes (internal)

Notes for deploying the demo. 

GitHub Pages only serves static files, so the C++ API runs separately on Render.

### 1. Deploy the API to Render
- Push this repo to GitHub first.
- On https://dashboard.render.com -> **New -> Blueprint**, pick the repo
  (the included `render.yaml` builds `Dockerfile`).
- After it finishes, copy the service URL, e.g.
  `https://strawcha-bank-api.onrender.com` (test `/api/health`).
- Free tier sleeps after inactivity, so the first request may take ~30s.

### 2. Point the web app at the API
In the GitHub repo: **Settings -> Secrets and variables -> Actions -> Variables**,
add a variable named `VITE_API_URL` with the Render URL value.

### 3. Enable GitHub Pages
**Settings -> Pages -> Build and deployment -> Source: GitHub Actions.**
The workflow `.github/workflows/deploy-pages.yml` then builds `web/` and
publishes it to `https://<user>.github.io/<repo>/` on every push to `main`.

> If you rename the repo, update `BASE_PATH` in the workflow (currently
> `/strawcha-bank/`). Locally, `npm run dev` still uses `BASE_PATH=/` and the
> Vite proxy to `:8080`, so nothing changes for development.
