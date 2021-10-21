#    NXGallery for Nintendo Switch
#    Made with love by Jonathan Verbeek (jverbeek.de)

@: build

dev:
	npm start

build: clean
	npm run build
	cp -r -f build/* ../app/romfs/www/

clean:
	rm -rf build/
	rm -rf ../app/romfs/www/*
	echo . > ../app/romfs/www/REACT_BUILD_GOES_HERE
