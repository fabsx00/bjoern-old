
Gremlin.defineStep('functionToBasicBlocks', [Vertex,Pipe], {
	_().transform{ q = "type:BasicBlock AND functionId:" + it.id; queryNodeIndex(q); }.scatter()
})
