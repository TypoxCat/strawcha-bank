import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// BASE_PATH lets GitHub Pages serve the app under /<repo>/ (e.g. /strawcha-bank/).
export default defineConfig({
  base: process.env.BASE_PATH || '/',
  plugins: [react()],
  server: {
    port: 5173,
    proxy: {
      '/api': {
        target: 'http://localhost:8080',
        changeOrigin: true
      }
    }
  },
  build: {
    outDir: 'dist'
  }
})
