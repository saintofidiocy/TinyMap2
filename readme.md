# TinyMap2.2
This project aims to clean up and update TinyMap2 features, especially since proper Remaster support has yet to be included in any official TinyMap2 release.


## Version History
- *1.2.0 - the version you are looking at here (WIP)*
  - Began rewriting code to be cleaner, more organized and easier to read (still work needing to be done, but trust me when I say this is *much* nicer than the code originally written in 2009)
  - Created github repo
  - Began adding support for OGG files
  - More things are planned!

#### Past Versions:
- 1.1.4b - A bunch of things that may or may not be a part of 1.1.4, but existed in hacked together private releases:
  - Fixed location counting
  - "Mission Briefing" action counted as Never
  - "Create Unit With Properties" and "Create Unit" actions merged
  - Creating 1 unit with Create Unit is now set to 0
  - TRIG section stacking (only applicable to consecutive duplicate triggers)
  - Updated buildable checking script to be more thorough (can only build buildings if you can have workers)
  - Hacked in STRx, CRGB, and Remaster versions
- 1.1.4 - Fixed Edmund Duke (Siege Mode) always being considered unused
- 1.1.3 - Clears unit data for units not actively used on the map (unplaced/unbuildable - such as those used only in death counters)
- 1.1.2 - No longer clears Consume energy cost, now clears Archon Warp and Lurker Aspect energies
- 1.1.1 - Fixed unit energy being set to 50/200, re-built latest sfmpqapi
- 1.1.0 - Added MPQ Block Size to the Options menu, removed Comment actions, removed Preserve Trigger actions
- 1.0.6 - Fixed Mission Briefing WAV counts, fixed Fog of War fix
- 1.0.5 - Fixed Modify Unit Resources removing new resource amount
- 1.0.4 - Fixed player 5-8 race clearing
- 1.0.3 - Fixed Terran Infantry Armor Max Level, fixed Fog of War
- 1.0.2 - Set default WAV compression to highest, fixed armor clearing, fixed seige tank/goliath damage clearing, fixed sprites being set to player 1
- 1.0.1 - Fixed 'Never' bug
- 1.0.0 - Initial Release
