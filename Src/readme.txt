HOW TO RENDER STUFF:

Follow the order
1. Create a model with constructor: Model NAME(ADDRESS OF OBJ FILE, STARTING POS)
2. Load textures with loadTextures(PNG IMAGE ADDRESS, TEX NUMBER)	
													//0 - Base color/Albedo		//1 - Normal map		//2 - Overlay texture
3. Generate the VAO with generateVAO()
4. Translate the object with translate(MODEL POSITION)
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