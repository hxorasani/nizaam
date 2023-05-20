# TODO

* rearrange src/, group files into subfolders and put readme/todo files in them
* port mudeer to nizaam as a web servermodule
	* this would require that nizaam windows be made optional
	* no window or opengl context should be created unless explicity required!
	* better memory management
	* sqlite support
* port slang to tarkeeb
	* views, softkeys etc would be really helpful
	* eventually video+audio tags can be supported!
	* tarkeeb should have a scene graph component with z-indexed elements
* develop support for dynamically loading libraries from files during runtime
	* this would make nizaam extremely extensible and dynamic!
* port nizaam to glfw3
* phase out libnuklear
	* need atlas based text rendering
* turn scripts/ into core/
	* this should only contain files that are almost always needed
* there sould be a modules/ folder that can potentially allow ``use_feature()``
	



