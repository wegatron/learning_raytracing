run:
	mkdir -p output
	cd build; ninja -j 2
	build/bin/zsw_one > output/image.ppm