# Lanyard Case

A one-piece edge cover with an open screen and back, two top lanyard tabs, and access to the side buttons, microSD, USB-C, charging LED, microphone, reset and original lanyard holes. Separate files provide a flexible full-lip variant and an experimental PLA variant with front retention ribs only on the two short edges.

## Print settings

Suggested starting settings, **not yet verified on a printed Sticky cover**:

- Material: TPU or flexible copolyester for `OuterCover`; PLA only for `OuterCover_PLA`.
- Layer height: 0.2 mm with a 0.4 mm nozzle.
- Walls: 4.
- Infill: 15%; thin walls and lips will mainly consist of perimeters.
- Supports: inspect the front lip and port bridges in the slicer; add local support where needed.
- Orientation: back face on the bed, matching the STL orientation. Units: mm.
- Temperature and speed: use the filament manufacturer's profile for your printer.

Print the matching bottom-30-mm `FitCoupon` first. It checks the bottom ports and microSD area but does not test full-device retention or the upper buttons. A user-supplied photograph shows a printed cover fitted on Sticky. Its filament and exact variant have not yet been confirmed, and button, port, retention and repeated-removal tests are not recorded.

## Assembly

1. Remove any brim and support, and check that the openings are clear.
2. For the flexible version, flex the side walls around the original device enclosure and seat the lip over the bezel without pressing the screen.
3. For PLA, engage one short edge under its rib and gently deflect the opposite case edge. This is not a rigid slide-in design; do not force it if the case cannot deflect enough.
4. Check button movement, cable insertion, card removal, and microphone/reset access. Test retention near a soft surface before using a lanyard.
5. To remove, gently release a short-edge rib by moving the case away from the device, without levering against the glass.

## Hardware

No screws, glue, inserts, or added magnets. An optional lanyard attaches to the two top tabs. The rear lip adds distance from a metal mounting surface; magnetic holding force needs a physical check.

## Files

https://github.com/ciniml/epaper-enclosures/tree/main/sticky

STL, STEP, dimensional parameters, generator source, and fit coupons for both variants are provided on the download page. The registry entry contains no model files.

## License

CC BY 4.0. Design by Kenta IDA / ciniml, based on the author's [M5Paper OuterCover](https://github.com/ciniml/M5Stack_Gadgets/tree/master/OuterCover_Paper). Port positions were cross-checked using Faulince Huang's reference case linked by Seeed; its mesh is not incorporated into these models.
