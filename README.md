I. CONTROLS:
	1 - Bullet
	2 - Artillery
	3 - Fireball
	4 - Laser
	Click - Fires selected projectile type
	Space - Resets cube position (excluding velocity)


II. DOCUMENTATION
A. Graphics
	- graphics_engine.cpp/h
	- config.h

B. Physics
   Particle/Rigidbody classes	- physics_engine.cpp/h
   Particle values to alter	- particle.h   
   Force Generators		- particle_force_generators.cpp/h
   Collisions			- particle_collisions.cpp/h
   Physics Logic		- particle_world.cpp/h

C. Controls
	-controls.cpp/h

D. Additional libraries
	- tiny_obj_loader.h
	- stb_image.h


III. HOW TO RENDER STUFF:
Follow the order
...
0. Create a particle.
1. Create a model with constructor: Model NAME(ADDRESS OF OBJ FILE, ADDRESS OF PARTICLE)
2. Load textures with loadTextures(PNG IMAGE ADDRESS, TEX NUMBER)	
													//0 - Base color/Albedo		//1 - Normal map		//2 - Overlay texture
3. Generate the VAO with generateVAO()
4. Copy the linked particle's transformMatrix with setTransformMatrix()
										//NEVER pass in a glm::vec3(0.f, 0.f, 0.f). It will not render
5.a Pack all the Camera, Lights, etc. into packedShader.packedClasses(......) 
	//If using the same light and camera, no need to repack packedClasses

5.b Pack all the active model properties into packedShader.modelProperties(......)
5.c Set the blendingMode and lightingMode from packedShader...
		//Blending modes
		//0 - No overlay texture
		//1 - With overlay texture
		//2 - Multiply textures

6. Render with drawSequence(packedShader)

7. Delete VAO and VBO before closing in glfwTerminate