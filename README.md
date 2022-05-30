Game!!

# Project Log

## Tuesday, 5/17, 4:15pm
**WHO:** Emily

**WHAT:** ported EVERYTHING

**BUGS:** none yet!

**RESOURCES USED:** none

## Wednesday, 5/18, 12:30am - 2:30am
**WHO:** Kaushik

**WHAT:** began setting up airhockey.c demo file

**BUGS:** so far, so good

**RESOURCES USED:** none

## Wednesday, 5/18 6:30pm - 8:00pm
**WHO:** Roy

**WHAT:** started planning and writing powerup functions

**BUGS:** none yet. Not really a bug but for whatever reason I wrote it as an h file. This file will disappear when I move it into the demo. DW. 

**RESOURCES USED:** none

## Wednesday, 5/18 - Thursday, 5/19 11:00pm - 2:15am
**WHO:** Avi and Kaushik

**WHAT:** Set up display for table, puck, players, and walls. 

**BUGS:** Walls aren't being displayed, and the puck is off-center at initialization. Also we need to do keyboard stuff. 

**RESOURCES USED:** none

## Thursday, 5/18 1:30pm - 2:15pm
**WHO:** Emily

**WHAT:** Set up powerup in state, trying to display walls. Fixed puck placement.

**BUGS:** Walls aren't being displayed, and the puck is off-center at initialization. Also we need to do keyboard stuff. 

**RESOURCES USED:** libraries: SDL_mixer 2.0 for sound, sdl ttf, sdl image

## Thursday, 5/18 11:00pm - 11:59pm
**WHO:** Avi and Kaushik

**WHAT:** Added goals, scoring, and checking win condition. 

**BUGS:**

**RESOURCES USED:** none

## Saturday, 5/21 11:00am - 1:15pm
**WHO:** Roy

**WHAT:** Attempted to make text rendering work. 

**BUGS:** Text rendering doesn't work - flags were added as appropriate but SDL functions are supposedly undefined despite being included. 

**RESOURCES USED:** SDL documentation. 

## Saturday, 5/21 11:00am - 1:15pm
**WHO:** Emily and Roy

**WHAT:** Attempted to make text rendering APPEAR. 

**BUGS:** SDL functions/libraries are recognized, but the text still does not appear in the demo.

**RESOURCES USED:** SDL documentation. 

## Sunday, 5/22 6:45pm - 8:15pm 
**WHO:** Roy

**WHAT:** Attempted to make freeze powerups spawn regularly

**BUGS:** emscripten is bugging tf out

**RESOURCES USED:** None

## Monday, 5/23 10:00am - 11:00am, 1:30pm - 2:00pm 
**WHO:** Roy

**WHAT:** Powerups spawn, freeze works. Ready to implement more powerups. 

**BUGS:** None!

**RESOURCES USED:** Stackoverflow - I forgot how to make function pointers. 

## Monday, 5/23 7:00pm - 8:30pm, 9:30pm - 10:30pm
**WHO:** Roy

**WHAT:** Powerups spawn. Randomized powerup spawning and implementation works! 

**BUGS:** Balancing needs some work... I think double accel and double velocity both enable bodies to go mach 10 or something... (9:30-10:30) no more mach 10! No more bugs currently. 

**RESOURCES USED:** None. 

## Tuesday, 5/24 9:00pm - 10:00pm
**WHO:** Kaushik

**WHAT:** Rewrote key handler using SDL_GetKeyboardState to accept multiple simultaneous key inputs and account for diagonal motion!

**BUGS:** The handler is a bit laggy... not sure how to resolve this

**RESOURCES USED:** SDL documentation

## Wednesday, 5/24 2:00am - 3:00am
**WHO:** Avi 

**WHAT:** Resolved the laggy handler. Added a cap on the amount of power-ups that appear on the screen at once. 

**BUGS:** Acceleration + min velocity might need tweaking. 

**RESOURCES USED:** 

## Thursday, 5/26 1:30pm - 3:30pm
**WHO:** Roy 

**WHAT:** Trying to implement text rendering. Lots of failed debugging. 

**BUGS:** The path for the font is not recognized, and "SDL not built with thread support, apparently". 

**RESOURCES USED:** StackOverFlow. 

## Thursday, 5/26 10pm - 11:30pm
**WHO:** Roy 

**WHAT:** Trying to implement text rendering. Lots of failed debugging. 

**BUGS:** The path for the font is not recognized, and "SDL not built with thread support, apparently". 

**RESOURCES USED:** None. No responses from TAs, either.  

## Friday, 5/27 8:30am - 10:45am
**WHO:** Roy 

**WHAT:** Implemented sound! 

**BUGS:** None. Our sound effects suck though. 

**RESOURCES USED:** SDL documentation, SDL_Mixer tutorials. 

## Friday, 5/27 8:00pm - 11:00pm
**WHO:** Roy 

**WHAT:** Made some balancing changes. Reset player positions and removed all powerup effects after a goal. Attempted to implement text and images, both to no avail. 

**BUGS:** Neither text nor images works. 

**RESOURCES USED:** SDL documentation and tutorials.

## Saturday, 5/28 9:00am - 11:15am
**WHO:** Roy 

**WHAT:** IMAGES WORK. And they stick to the body too. 

**BUGS:** No text. Oh well. 

**RESOURCES USED:** SDL documentation and tutorials.

## Saturday, 5/28 9:30pm - 10:00pm
**WHO:** Roy 

**WHAT:** SCOREBOARD

**BUGS:** No text. Haven't tried. 

**RESOURCES USED:** None.

## Sunday, 5/29 12:30pm - 2:30pm
**WHO:** Kaushik 

**WHAT:** Attempted to convert all info into enums to avoid confusion and potential memory leaks. Also
re-rewrote the key handler so that the resultant motion of the paddles is far more intuitive.

**BUGS:** Enums don't work due to inconsistencies between void * and enum types. I just resolved the
numerous warnings regarding our info types instead - now we no longer have warnings.

**RESOURCES USED:** None.