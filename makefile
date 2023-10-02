# run:
# 	mkdir -p output
# 	cd build; ninja -j 2
# 	build/bin/zsw_one > output/image.ppm

run:
	mkdir -p output
	cd build_release; make -j
	build_release/bin/zsw_one > output/image.ppm