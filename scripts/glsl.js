var attributes, uniforms, glsl = {};
;(function(){ // glsl
	var path = Nizaam.scripts_path;
	var pre = sgl.create_program(path+"shaders/v.glsl", path+"shaders/f.glsl");
	if (undefined == pre) log('error pre program');

	var post = sgl.create_program(path+"shaders/post.v.glsl", path+"shaders/post.f.glsl");
	if (isundef(post)) log('error post program');

	attributes = {
		coord3d: sgl.get_attribute("coord3d", pre),
		v_color: sgl.get_attribute("v_color", pre),
		v_normal: sgl.get_attribute("v_normal", pre),
//		coord2dpre: sgl.get_attribute("coord2d", pre),
		textu2dpre: sgl.get_attribute("textu2d", pre),

		coord2d: sgl.get_attribute("coord2d", post),
		textu2d: sgl.get_attribute("textu2d", post),
	};
	uniforms = {
		m_3x3_inv_transp : sgl.get_uniform("m_3x3_inv_transp", pre),
		sun_pos		 : sgl.get_uniform("sun_pos", pre),
		sun_dir		 : sgl.get_uniform("sun_dir", pre),
		lighting	 : sgl.get_uniform("lighting", pre),
		selected	 : sgl.get_uniform("selected", pre),
		single_color : sgl.get_uniform("single_color", pre),
		has_color	 : sgl.get_uniform("has_color", pre),
		fogColor	 : sgl.get_uniform("fogColor", pre),
		model		 : sgl.get_uniform("model", pre),
		transform	 : sgl.get_uniform("transform", pre),
		ortho		 : sgl.get_uniform("ortho", pre),
		image_tex	 : sgl.get_uniform("screen_tex", pre),
		show_vertices: sgl.get_uniform("show_vertices", pre),
		picking_mode : sgl.get_uniform("picking_mode", pre),
		object_id	 : sgl.get_uniform("object_id", pre),

		screen_tex	 : sgl.get_uniform("screen_tex", post),
	};

	sgl.uniform_4f(uniforms.single_color, 1, 1, 1, 1);

	glsl.samples  = 8;

	glsl.pre  = pre ;
	glsl.post = post;

	glsl.fb = sgl.framebuffer(); // offscreen frame buffer
	glsl.ib = sgl.framebuffer(); // intermediate frame buffer
	glsl.pb = sgl.framebuffer(); // picking frame buffer
	
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	glsl.quad_va = sgl.vertex_array();
	sgl.active_texture(sgl.GL_TEXTURE0);
	glsl.quad = sgl.array_buffer (new Float32Array([
		//coord, texture
        -1,  1,  0, 1,
        -1, -1,  0, 0,
         1, -1,  1, 0,

        -1,  1,  0, 1,
         1, -1,  1, 0,
         1,  1,  1, 1
    ]) );
	sgl.enable_array_buffer(attributes.coord2d, glsl.quad, 2, 4, 0);
	sgl.enable_array_buffer(attributes.textu2d, glsl.quad, 2, 4, 2);
	sgl.bind(sgl.GL_VERTEX_ARRAY, 0);
	sgl.disable_array_buffer(attributes.coord2d);
	sgl.disable_array_buffer(attributes.textu2d);

	hooks.set('nizaam.resize', function (w, h) {
		if (isundef(glsl.fb) || isundef(glsl.ib) || isundef(glsl.pb)) return;
		
		sgl.active_texture(sgl.GL_TEXTURE0);

		sgl.bind(sgl.GL_FRAMEBUFFER, glsl.pb);
		
		glsl.pick = sgl.texture(glsl.pick, sgl.GL_TEXTURE_2D, 0, sgl.GL_R32F, w, h, sgl.GL_RGB, sgl.GL_FLOAT);
		sgl.bind(sgl.GL_TEXTURE_2D, 0);
		sgl.attach_framebuffer(sgl.GL_TEXTURE_2D, sgl.GL_COLOR_ATTACHMENT0, glsl.pick);

		glsl.pick_depth = sgl.texture(glsl.pick_depth, sgl.GL_TEXTURE_2D, 0, sgl.GL_DEPTH_COMPONENT, w, h, sgl.GL_DEPTH_COMPONENT, sgl.GL_FLOAT);
		sgl.bind(sgl.GL_TEXTURE_2D, 0);
		sgl.attach_framebuffer(sgl.GL_TEXTURE_2D, sgl.GL_DEPTH_ATTACHMENT, glsl.pick_depth);
		
		sgl.read_buffer(sgl.GL_NONE);
		sgl.draw_buffer(sgl.GL_COLOR_ATTACHMENT0);

		var status = sgl.check_framebuffer();
		if (status !== 1) log("picking frame buffer incomplete "+status);
		sgl.bind(sgl.GL_FRAMEBUFFER, 0);

		sgl.bind(sgl.GL_FRAMEBUFFER, glsl.fb);

		// create a multisampled color attachment texture
		glsl.msaa = sgl.texture(glsl.msaa, sgl.GL_TEXTURE_2D_MULTISAMPLE, glsl.samples, sgl.GL_RGB, w, h);
		sgl.bind(sgl.GL_TEXTURE_2D_MULTISAMPLE, 0);
		sgl.attach_framebuffer(sgl.GL_TEXTURE_2D_MULTISAMPLE, sgl.GL_COLOR_ATTACHMENT0, glsl.msaa);

		// create a (also multisampled) renderbuffer object for depth and stencil attachments
		glsl.rbo = sgl.renderbuffer(glsl.rbo, glsl.samples, sgl.GL_DEPTH24_STENCIL8, w, h);
		sgl.bind(sgl.GL_RENDERBUFFER, 0);
		sgl.attach_framebuffer(sgl.GL_RENDERBUFFER, sgl.GL_DEPTH_STENCIL_ATTACHMENT, glsl.rbo);

		status = sgl.check_framebuffer();
		if (status !== 1) log("offscreen frame buffer incomplete "+status);
		sgl.bind(sgl.GL_FRAMEBUFFER, 0);

		// configure second post-processing framebuffer
		sgl.bind(sgl.GL_FRAMEBUFFER, glsl.ib);
		// create a color attachment texture
		glsl.scr = sgl.texture(glsl.scr, sgl.GL_TEXTURE_2D, 0, sgl.GL_RGB, w, h, sgl.GL_RGB, sgl.GL_UNSIGNED_BYTE, 0);
		sgl.bind(sgl.GL_TEXTURE_2D, 0);
		sgl.attach_framebuffer(sgl.GL_TEXTURE_2D, sgl.GL_COLOR_ATTACHMENT0, glsl.scr);

		status = sgl.check_framebuffer();
		if (status !== 1) log("intermediate framebuffer incomplete "+status);
		sgl.bind(sgl.GL_FRAMEBUFFER, 0);
	});
	hooks.set('nizaam.destroy', function () {
		sgl.delete_program(glsl.pre );
		sgl.delete_program(glsl.post);
	});
	
	set_timeout(1, function () {
		hooks.run('glsl.ready');
	});
})();
