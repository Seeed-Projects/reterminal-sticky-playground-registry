# SolarOS

Turn your reTerminal Sticky into a pocket computer with SolarOS: a powerful terminal, Python and Lua scripting, and ready-to-run community applications.

This Seeed-coordinated partner entry opens the official external installer.
The supplied SolarOS logo is used as partner identity artwork.

- Flash: https://solar-os.eu/flasher.html?board=seeed_reterminal_sticky&flavor=full&layout=ota
- About: https://solar-os.eu/
- Docs & Guide: https://solar-os.eu/docs/manual/overview.html
- Source: https://github.com/nilseuropa/solar_os
- License: Apache-2.0, confirmed from the upstream LICENSE.md.
- Support: https://github.com/nilseuropa/solar_os/issues

## Installation and expected result

1. Back up device settings and files. Installation erases all internal flash, including Wi-Fi credentials and applications.
2. Connect Sticky using a USB data cable and open the Flash link in desktop Chrome or Edge.
3. Verify the board is `seeed_reterminal_sticky`, the flavor is `full`, and the layout is `ota`.
4. Prepare the image, acknowledge the erase warning, choose Connect and flash, and grant access to the correct USB serial port.
5. Keep the device connected until installation finishes. Follow the manual after reboot to use the terminal, applications, Python and Lua.

Expected result: Sticky boots SolarOS and provides its terminal and available applications. The Seeed contributor confirmed that this workflow was tested on reTerminal Sticky.

## External delivery

Seeed-coordinated partner entry using the maintained SolarOS browser flasher. Upstream supplies firmware images; this entry deliberately links to its board, flavor and OTA layout selection rather than mirroring a fixed binary package in the Registry.

## Requirements and data handling

Desktop Chrome or Edge and USB serial-port permission are recommended. Installation erases all internal flash, including credentials and files; back up first. The flasher states that nothing is uploaded from the computer. The Seeed contributor confirmed review and testing of the external installation workflow and service requirements.

## Verification and maintenance

Partner authorization, physical-device testing, the parameterized installer workflow and service requirements were confirmed by the Seeed contributor. This entry is maintained as a Seeed-coordinated partner integration.

Report issues through the upstream support link. Submit destination or service changes for review. No firmware binaries or build configuration are included in this external entry.
