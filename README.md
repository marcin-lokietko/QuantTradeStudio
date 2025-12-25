# General description

# Prerequisites

Install docker and docker compose on your system.

# Keys setup

## Binance testnet

Create apiKey.txt and secretKey.txt containing keys generated on https://testnet.binance.vision/
Then copy those txt files into catalog.

# Development

## Building Docker images

Execute

```
./scripts/build_docker_images.sh
```

in command line

## Building backend using VS code

1. Connect to backend Docker container using .devcontainer/devcontainer.json configuration
2. Execute task "Build debug" defined in .vscode/tasks.json (this is the default build, so you can just click Ctrl + Shift + b)

# Launching

## Run in development mode

Execute

```
./scripts/run_devel.sh
```

in command line

## Run in development mode using VS code for backend debugging

1. Connect to backend Docker container using .devcontainer/devcontainer.json configuration
2. Execute "Launch main debug" debug config defined in .vscode/launch.json (this is the default task, so you can just click F5)
3. Execute

```
./scripts/run_frontend_in_docker.sh
```

in command line.

Now you can use gdb debugger while running backend connected to frontend.

# Documentation

- doc/Architecture.md - high-level architecture description
- doc/ArchitectureLog.md - design changes done during the development

# Further development plans

# Disclaimer

This project is provided for educational and demonstration purposes only. It is not financial advice and is not intended for live trading.

The author makes no guarantees of performance, profitability, or accuracy, and accepts no liability for any losses arising from the use of this software. Use at your own risk.
