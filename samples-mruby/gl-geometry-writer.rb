# MOAIDebugLinesMgr.setStyle( MOAIDebugLinesMgr::PROP_MODEL_BOUNDS, 2, 1, 1, 1 )
# MOAIDebugLinesMgr.setStyle( MOAIDebugLinesMgr::PROP_WORLD_BOUNDS, 1, 0.5, 0.5, 0.5 )

# frameBuffer = MOAIGfxMgr.getFrameBuffer()
# frameBuffer.setClearDepth( true )

MOAISim.openWindow( "test", 320, 480 )

viewport = MOAIViewport.new
viewport.setSize( 320, 480 )
viewport.setScale( 320, 480 )

layer = MOAIPartitionViewLayer.new
layer.setViewport( viewport )
layer.pushRenderPass()

camera = MOAICamera.new
camera.setLoc( 0, 0, camera.getFocalLength( 320 ))
layer.setCamera( camera )

makeCube = proc do | size |
	
	y = 0

	vtxFormat = MOAIVertexFormatMgr.getFormat( MOAIVertexFormatMgr::XYZC )

	vtxStream = MOAIMemStream.new
	idxStream = MOAIMemStream.new

	for i in 1..16 do

		hSize = size * 0.5

		MOAIGeometryWriter.writeCube( vtxFormat, vtxStream, size, 0, y, 0 )

		MOAIGeometryWriter.applyLinearGradient( vtxFormat, vtxStream,
			-hSize, y - hSize, -hSize,
			hSize, y + hSize, hSize,
			0, 0, 0, 1,
			1, 0, 0, 1,
			false,
			false,
			MOAIGeometryWriter::COLOR_OVERWRITE
		)
		
		vtxStream.seek( 0, MOAIMemStream::SEEK_END )

		size = hSize
		y = y + ( size * 1.5 )
	end

	vtxStream.seek( 0 )
	MOAIGeometryWriter.snapCoords( vtxFormat, vtxStream, 0.1 )
	MOAIGeometryWriter.pruneVertices( vtxFormat, vtxStream, idxStream )

  mesh = MOAIGeometryWriter.getMesh( vtxFormat, vtxStream, nil, idxStream, 2 )
  puts mesh
	mesh.setShader( MOAIShaderMgr.getShader( MOAIShaderMgr::LINE_SHADER_3D ))

	return mesh
end

mesh = makeCube.call( 128 )

prop = MOAIGraphicsProp.new
prop.setDeck( mesh )
prop.moveRot( 360, 0, 360, 6 )
prop.setCullMode( MOAIGraphicsProp::CULL_BACK )
prop.setDepthTest( MOAIGraphicsProp::DEPTH_TEST_LESS )
prop.setPartition( layer )