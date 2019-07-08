
callback = proc do | name |
  puts "update #{name}"
end

makeNode = proc do | name |
	node = MOAINode.new
	node.setListener( MOAINode::EVENT_NODE_POST_UPDATE, callback.call( name ) )
	node
end

a = makeNode.call( 'A' )
b = makeNode.call( 'B' )
c = makeNode.call( 'C' )
d = makeNode.call( 'D' )
e = makeNode.call( 'E' )

b.setNodeLink( a )
c.setNodeLink( b )

a.scheduleUpdate()
c.forceUpdate()
