# Deploy notes (internal)

Notes for deploying the demo.

GitHub Pages only serves static files, so the C++ API runs separately on
**Hugging Face Spaces** (Docker, free, no credit card). Render works too but
currently asks new accounts for a credit card for identity verification.

### 1. Deploy the API to Hugging Face Spaces
- Push this repo to GitHub first, and make it **public** (the Space clones it).
- Go to https://huggingface.co/new-space and create a Space:
  - **SDK**: Docker -> Blank
  - **Hardware**: CPU basic (free)
  - **Visibility**: Public
  - Name it e.g. `strawcha-bank-api`.
- Upload the two files from `deploy/huggingface/` to the Space
  (`README.md` and `Dockerfile`) via the Space **Files -> Add file** tab.
  The Dockerfile clones the GitHub repo and builds the API on port `7860`.
- The Space URL is `https://<user>-<space-name>.hf.space`. Test
  `/api/health` (should return `{"status":"ok"}`).
- Rebuild the Space after code changes (Space **Settings -> Factory rebuild**).

### 2. Point the web app at the API
In the GitHub repo: **Settings -> Secrets and variables -> Actions -> Variables**,
add a variable named `VITE_API_URL` with the Space URL, e.g.
`https://<user>-strawcha-bank-api.hf.space`.

### 3. Enable GitHub Pages
**Settings -> Pages -> Build and deployment -> Source: GitHub Actions.**
The workflow `.github/workflows/deploy-pages.yml` then builds `web/` and
publishes it to `https://<user>.github.io/<repo>/` on every push to `main`.

> If you rename the repo, update `BASE_PATH` in the workflow (currently
> `/strawcha-bank/`). Locally, `npm run dev` still uses `BASE_PATH=/` and the
> Vite proxy to `:8080`, so nothing changes for development.

### Alternative: Render
`render.yaml` + `Dockerfile` are kept for Render (New -> Blueprint). Free
instances sleep after inactivity (~30s cold start) and may require a card.
