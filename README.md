# nizaam (arabic for: system) is a linux-based OS for the next generation.


## Long Term Goals
* use the latest open standards
* remain minimal, in MBs and not GBs
* any dependencies should be pre-packaged
* be accessible to less-abled humans
* should work on most hardware with the least changes
* should work inside other OSes (android, macos, win, ...)
* be user and developer friendly
* be able to run Mudeer apps

## Mid Term Goals
* **DONE** run on [X11](https://en.wikipedia.org/wiki/X_Window_System)
* **DONE** run on [Wayland](https://wayland.freedesktop.org/)
* **TODO** run on [DRM](https://en.wikipedia.org/wiki/Direct_Rendering_Manager)
* **TODO** implement an Audio API
* **TODO** implement a Fonts API

## Short Term Goals
Found in [TODO](https://github.com/xorasan/nizaam/edit/master/TODO.md)

## Why
why have another display server or protocol when there's open source alternativs that exist already
* display servers
	* X is too old
	* wayland/wlroots seems good but the build system is burdensome
		* also having to keep up with the specs can be difficult
		* i'm still willing to implement it as a secondary optional backend
	* nizaam requires that the display server be portable
		* a good hint is the ability to run on Windows & Android directly
		* minimal, few lines of code and get out of the way
		* be extendable but also backwards compatible
		* barebones to the point you can feel the metal (aka hardware)



## game
should be built on these principles
* tiny, the core should be few MBs at worse
	* this includes, physics, 2d, 3d, text, photos, videos, animations, sound
* super fast even on slightly older smaller hardware like budget phones
* promote good values, genuinely beneficial, never waste people's time
* give away knowledge through adventures
	* so something you have to do in real life, you can do in the game
	* and learn the same lessons for cheap
* be entertaining on multiple levels, expected & surprising
* sometimes talk to the player directly, people shouldn't get lost in the game
	* build the 4th wall & break it, rinse, repeat.
* should not be based on input gymnastics
	* pressing certain keys fast enough
	* certain pointer or touch gestures fast enough


## video app
* the target is to produce videos
* physics should be built in
	* gravity and each object falls
	* velocity and acceleration
	* these should be applied to all objects by default
	* they can be adjust for each object and turned off too (freeform mode)
* it should be great on multiple levels
* not only should it be fast but also produce small sized files
* it should provide tools to conceptualize videos super fast
* and it should deal in the concrete and the metaphor
* it should treat time & space as primary components to vid production
* it should have bars for time with names, colors and icons
	* like blink, flash, glance, stare, ...
* it should also know about who you are aiming the video at
	* and what times and contexts, bg, fg
* it should be modular, so that people can write plugins/effects for it
	* the opport for an effects store
	* assets store with standards
		* no asset should slow down the app
* UI priority: color, intensity, shape, icon, text
* single handed use



## helpful while development
* while doing duktape shared library installs, i had to do ldconfig to cache
  the newly installed shared libs for them to be found




