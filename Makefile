#    NXGallery for Nintendo Switch
#    Made with love by Jonathan Verbeek (jverbeek.de)

#	IP Address of the Switch, change this to the address in your network
# 	Used for "make upload"
SWITCH_IP_ADDR = 192.168.178.46

#---------------------------------------------------------------------------------

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

#	Transfers the built homebrew app over to the console by using nxlink
upload:
	nxlink -s -a $(SWITCH_IP_ADDR) out/switch/NXGallery.nro

#	Cleans everything
clean:
	@rm -rf out/
	@$(MAKE) clean -C app/

#---------------------------------------------------------------------------------
