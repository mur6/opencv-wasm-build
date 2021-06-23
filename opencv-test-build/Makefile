build-bulk-memory:
	bash ./run_docker.sh build_wasm
build-no-bulk-memory:
	bash ./run_docker.sh build_wasm_no_bulk_memory
run:
	/usr/local/bin/node build_wasm/main.js
objdump:
	wasm-objdump -h build_wasm/main.wasm
clean:
	rm -rf build_wasm/
