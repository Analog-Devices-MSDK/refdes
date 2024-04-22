# App Switcher

<br>

MAXREFDES178# has a special application switcher bootloader. App-Switcher can load MAX32666, MAX78000-Video and MAX78000-Audio demo firmware using a micro SD card.

<div style="margin-left: 0; background-color: #dfd;" >

**Warning:** The MAXREFDES178# App-Switcher is available from version **v1.1.67** and later. Please update MAX32666, MAX78000-Video and MAX78000-Audio firmware with the FaceID demo with version **v1.1.67** (or later) by following the steps in the “Load Demo Firmware Using MINGW on Windows” section.

</div>

**NOTE:** App-Switcher is embedded in the demo firmware. It is not necessary to load App-Switcher binaries separately.

App-Switcher uses **msbl** files to load MAX32666, MAX78000-Video and MAX78000-Audio firmware from a micro SD card. **msbl** files can be found in `build` directory when the build is completed. Alternatively, you can download the latest **.msbl** files from GitHub Release or GitHub Actions.

<br><br>

## Preparing an SD Card

<br><br>


* Format the micro SD card with **FAT32** file system. **Make sure to back up your SD Card content before formatting!**

* Download the latest maxrefdes178_firmware.zip release from:

  * https://github.com/analogdevicesinc/MAX78xxx-RefDes/releases for stable releases.
  * https://github.com/analogdevicesinc/MAX78xxx-RefDes/refdes/actions/workflows/maxrefdes178.yml for GitHub Action builds.

* Extract zip content to the micro SD card root directory.

<p align="center"><img src="images/wiki_appswitchersdcard.png" width="300" alt="MAXDAP TYPE-C" title=""></p>

App-Switcher SD Card folder structure requirements:

  * All demo directories must be in the root directory of the SD card.
  * All demo directories must contain these files:
       `maxrefdes178_max32666_demo.msbl`
       `maxrefdes178_max78000_video_demo.msbl`
       `maxrefdes178_max78000_audio_demo.msbl`

<br><br>

## Running App-Switcher

* Turn off the device by pressing the **power button**.
* Insert the micro SD card.
* While pressing **Button X**, press **power button** to turn on the device.
* The device will start in App-Switcher mode.
* If there isn't a valid demo on MAX32666 flash, App-Switcher will start automatically.

<br><br>

## Loading a Demo From an SD Card

<br><br>

* Pressing **Button X** changes the selected demo. The selected demo will be highlighted in Green.

  <p align="center"><img src="images/wiki_appswitcherdemolist.jpg" width="400" alt="MAXDAP TYPE-C" title=""></p>



* **Button Y** starts the firmware update procedure for the selected demo.

<br>

  <p align="center"><img src="images/wiki_appswitcherdemoload.jpg" width="400" alt="MAXDAP TYPE-C" title=""></p>

<br>

* First, App-Switcher loads MAX78000-Audio firmware.

<br>

  <p align="center"><img src="images/wiki_appswitcherdemoloadaudio.jpg" width="400" alt="MAXDAP TYPE-C" title=""></p>

<br>

* Then, App-Switcher loads MAX78000-Video firmware.

<br>

  <p align="center"><img src="images/wiki_appswitcherdemoloadvideo.jpg" width="400" alt="MAXDAP TYPE-C" title=""></p>

<br>

* And last, App-Switcher loads MAX32666 firmware.

<br>

  <p align="center"><img src="images/wiki_appswitcherdemoloadmax32666.jpg" width="400" alt="MAXDAP TYPE-C" title=""></p>

<br>


* If all three firmware updates are successful, the device will restart with the selected demo.

<br>

  <p align="center"><img src="images/wiki_appswitcherdemoloaded.jpg" width="400" alt="MAXDAP TYPE-C" title=""></p>

<br>

* If the micro SD card is not inserted, App-Switcher halts and waits for a restart. Press the power button for 12 seconds to turn off the App-Switcher.

<br>

  <p align="center"><img src="images/wiki_appswitchernocardfound.jpg" width="400" alt="MAXDAP TYPE-C" title=""></p>

<br>

* If the demo folder content is invalid (incorrect or missing **msbl** files), App-Switcher returns to the main menu.

<br>

  <p align="center"><img src="images/wiki_appswitcherinvalidcard.jpg" width="400" alt="MAXDAP TYPE-C" title=""></p>

<br>

<br><br><br>


<br><br><br>
<div class="nextpage" style="margin-left: 0; margin-right: auto; text-align: right; background-color: #dfd;" >
NEXT : <a href="RecoveryProcedure.md">Recovery Procedure</a>
</div>


