
Gremlin.defineStep('functionToBasicBlocks', [Vertex,Pipe], {
	_().transform{ q = "type:BasicBlock AND functionId:" + it.id; queryNodeIndex(q); }.scatter()
})

Gremlin.defineStep('functionToInstructions', [Vertex,Pipe], {
	_().functionToBasicBlocks().out().filter{it.type == "Instruction"}
})
