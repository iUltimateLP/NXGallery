#    NXGallery for Nintendo Switch
#    Made with love by Jonathan Verbeek (jverbeek.de)

#	IP Address of the Switch, change this to the address in your network
# 	Used for "make upload"
SWITCH_IP_ADDR = 192.168.178.46

#---------------------------------------------------------------------------------

TITLE_ID_APP = 4200000000000909
TITLE_ID_SYSMODULE = 4300000000000909

#---------------------------------------------------------------------------------
#	Scripts

# 	Phony target
.PHONY: all app sysmodule stage clean

# 	Build all
all: app sysmodule
	@:

#	Build the application
app:
	@$(MAKE) -C $@
	@$(MAKE) stageApp

#	Build the sysmodule
sysmodule:
	@$(MAKE) -C $@
	@$(MAKE) stageSysmodule

#	Stage the release into one single folder which can be copied on the SD card
stageApp:
	@mkdir -p out/switch/
	@cp app/out/app.nro out/switch/NXGallery.nro

stageSysmodule:
	@mkdir -p out/atmosphere/contents/$(TITLE_ID_SYSMODULE)/flags
	@touch out/atmosphere/contents/$(TITLE_ID_SYSMODULE)/flags/boot2.flag
	@cp sysmodule/out/sysmodule.nsp out/atmosphere/contents/$(TITLE_ID_SYSMODULE)/exefs.nsp

#	Transfers the built homebrew app over to the console by using nxlink
upload:
	nxlink -s -a $(SWITCH_IP_ADDR) out/switch/NXGallery.nro

#	Cleans everything
clean:
	@rm -rf out/
	@$(MAKE) clean -C app/
	@$(MAKE) clean -C sysmodule/

#---------------------------------------------------------------------------------
