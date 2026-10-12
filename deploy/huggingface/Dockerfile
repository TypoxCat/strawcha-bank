# Hugging Face Docker Space untuk API Strawcha Bank (C++).
# Space ini clone repo GitHub (publik) lalu build API-nya, agar tidak
# menduplikasi sumber kode. Ganti REPO_URL bila nama repo berbeda.
FROM gcc:13 AS build
ARG REPO_URL=https://github.com/TypoxCat/strawcha-bank.git
RUN apt-get update \
    && apt-get install -y --no-install-recommends git ca-certificates \
    && rm -rf /var/lib/apt/lists/*
RUN git clone --depth 1 "$REPO_URL" /src
WORKDIR /src
RUN make clean && make mbanking-server \
    CXXFLAGS="-std=c++17 -O2 -I. -Iserver -Isrc -static-libstdc++ -static-libgcc"

FROM debian:bookworm-slim
RUN apt-get update \
    && apt-get install -y --no-install-recommends libstdc++6 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /src/mbanking-server ./
ENV PORT=7860
EXPOSE 7860
CMD ["./mbanking-server"]
