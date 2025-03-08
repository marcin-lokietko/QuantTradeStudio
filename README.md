# Development
## Building Docker images
Execute
```
./scripts/build_docker_images.sh
```
in command line

## Building backend using VS code
1. Connect to backend Docker container using .devcontainer/devcontainer.json configuration
2. Execute task "Build debug" defined in .vscode/tasks.json

# Launching
## Run in development mode
Execute
```
./scripts/run_devel.sh
```
in command line

## Run in development mode using VS code for backend debugging
1. Connect to backend Docker container using .devcontainer/devcontainer.json configuration
2. Execute "Launch main debug" debug config defined in .vscode/launch.json
3. Execute
```
./scripts/run_frontend_in_docker.sh
```
in command line.

Now you can use gdb debugger while running backend connected to frontend.
