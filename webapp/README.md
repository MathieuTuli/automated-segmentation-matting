# readme

## usage
1. go to `internal/webgui` and run `make`
  - this will generate `build/imgui.{js,data,wasm}`
1. copy those files to `web/js/`
1. edit `web/js/imgui.js` and update the path for `imgui.data` to be `/app/imgui.data`
1. run `go mod init asm-app` and `go mod tidy` and `go install github.com/cosmtrek/air@latest`
1. run the go app `go run cmd/main.go` (install air also and run `air` if you want)

## todo
- currently, you can compile the imgui app in `internal/webgui` - but the paths get screwed up when you copy the `imgui.{js,data,wasm}` files over to the webapp. it's required to edit `imgui.js` to make the path `/app/imgui.data`.
  - will fix this with proper methods later
