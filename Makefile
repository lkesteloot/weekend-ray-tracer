
.PHONY: gif
gif:
	convert -loop 0 -delay 3 anim/out-*.png out.gif

.PHONY: mp4
mp4:
	ffmpeg -pattern_type glob -i 'anim/*.png' -pix_fmt yuv420p -y out.mp4

.PHONY: docker-build
docker-build: $(BIN)
	docker build -t ray .

.PHONY: docker-run
docker-run:
	docker run -it --name ray-run ray

.PHONY: docker-get
docker-get:
	docker cp ray-run:out.png out.png

