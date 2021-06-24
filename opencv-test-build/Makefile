build-3-bulk-memory:
	bash ./run_docker.sh 3 build_wasm
build-3-no-bulk-memory:
	bash ./run_docker.sh 3 build_wasm_no_bulk_memory
build-4-bulk-memory:
	bash ./run_docker.sh 4 build_wasm
build-4-no-bulk-memory:
	bash ./run_docker.sh 4 build_wasm_no_bulk_memory
run:
	/usr/local/bin/node build_wasm/main.js
objdump:
	wasm-objdump -h build_wasm/main.wasm
clean:
	rm -rf build_wasm/
