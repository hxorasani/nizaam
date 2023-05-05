# TODO

* rearrange src/, group files into subfolders and put readme/todo files in them
* port mudeer+slang to nizaam:tarkeeb
	* views, softkeys etc would be really helpful اللّه
	* eventually video+audio tags can be supported! سانه رائع جدًا
* calc text extents and wordwrap text
	* then make it efficient
	* then port kaatib to cairo
* tarkeeb should have a scene graph component with z-indexed elements
	* when a higher visible element reports damage, the lower ones should redraw
	  in the area reported by it, include the wijhah background
	* i think this is where cairo masking can really make dev easier
		* setting higher elements as the negative mask
		* then drawing the actual element using cairo
		* that might keep it efficient without havng to handle masking myself
* fig out how to have dynamically linked dependency libs like cairo
	* linux c dynamic link libs
* i removed (hadaf, ...), the first arg from qumaac, do the same for msfoof.*

## IDEAs
* wayland-nizaam, run wayland apps inside nizaam
	* (wlroots) [https://gitlab.freedesktop.org/wlroots/wlroots]
		* tried building it, really bad experience
			* update meson, wayland libs, libdrm, erors. :(
	* (tiny-wayland-compositor) also depends on wlroots :(
	* no changes needed to the apps
	* each app appears as an amr to nizaam
	* sub-windows are opened in the same raees

