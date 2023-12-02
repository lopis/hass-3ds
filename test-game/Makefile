ifdef DEVKITARM
include /usr/src/app/build-makefile.mk
endif

build-docker:
	docker build -t test_game . ; \
	mkdir -p dist ; \
	docker create --name temp_container test_game ; \
	docker cp temp_container:/usr/src/app/app.3dsx ./dist/app.3dsx ; \
	docker rm temp_container