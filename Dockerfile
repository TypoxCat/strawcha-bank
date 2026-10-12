# ── Build stage ──
FROM gcc:13 AS build
WORKDIR /app
COPY Makefile ./
COPY src ./src
COPY server ./server
RUN make mbanking-server

# ── Runtime stage ──
FROM debian:bookworm-slim
RUN apt-get update \
    && apt-get install -y --no-install-recommends libstdc++6 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/mbanking-server ./
ENV PORT=8080
EXPOSE 8080
CMD ["./mbanking-server"]
