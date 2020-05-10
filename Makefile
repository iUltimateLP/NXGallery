#    NXGallery for Nintendo Switch
#    Made with love by Jonathan Verbeek (jverbeek.de)

TITLE_ID = 4200000000000909

#---------------------------------------------------------------------------------
#	Scripts

# 	Phony target
.PHONY: all app stage clean

# 	Build all
all: app
	@:

#	Build the application
app:
	@$(MAKE) -C $@
	@$(MAKE) stage

#	Stage the release into one single folder which can be copied on the SD card
stage:
	mkdir -p out/switch/
	cp app/out/app.nro out/switch/NXGallery.nro

#	Cleans everything
clean:
	@rm -rf out/

#---------------------------------------------------------------------------------
