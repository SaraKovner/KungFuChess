#!/bin/bash

echo "🐳 Building KungFu Chess Docker Images..."

# Build server image
echo "📦 Building server image..."
docker build -f Dockerfile.server -t kungfu-chess-server .

if [ $? -eq 0 ]; then
    echo "✅ Server image built successfully!"
else
    echo "❌ Server build failed!"
    exit 1
fi

# Build client image
echo "📦 Building client image..."
docker build -f Dockerfile.client -t kungfu-chess-client .

if [ $? -eq 0 ]; then
    echo "✅ Client image built successfully!"
else
    echo "❌ Client build failed!"
    exit 1
fi

echo "🎉 All images built successfully!"
echo "📋 Available images:"
docker images | grep kungfu-chess