#!/usr/bin/env bash
sed -i "s/\${ADDR}/${ADDR}/g" /app/config/blyat.toml
sed -i "s/\${PORT}/${PORT}/g" /app/config/blyat.toml
/app/blyat /app/config/blyat.toml ${THREADS}
