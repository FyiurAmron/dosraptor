# User-side changes
 * made srand() configurable (either setup.ini or time)
 * exposed gdmode, taiwan and birthday via setup.ini
 * added 7th birthday monkey and random monkey
 * added quick mode (skip intros, confirmations, shop fluff etc.)
 * added logging to file (in progress!)
 * changed cursor behaviour: originally, after entering hangar, shop,
   "NEW GAME", "OPTIONS" or choice box the cursor jumps to "active" hotspot, but only
   visually and logically stays, and as a result jumps to center again after
   moving the cursor even slightly - this wackiness has been removed completely