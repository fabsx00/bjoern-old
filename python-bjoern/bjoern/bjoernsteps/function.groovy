
Gremlin.defineStep('functionToBasicBlocks', [Vertex,Pipe], {
	_().transform{ q = "type:BasicBlock AND functionId:" + it.id; queryNodeIndex(q); }.scatter()
})

Gremlin.defineStep('functionToInstructions', [Vertex,Pipe], {
	_().functionToBasicBlocks().out().filter{it.type == "Instruction"}
})

Gremlin.defineStep('functionToCalls', [Vertex,Pipe], {
	_().functionToInstructions().filter{it.code.startsWith("call") }
})

Gremlin.defineStep('functionToCallers', [Vertex,Pipe], {
	_().sideEffect{ funcAddr = it.addr }
	.transform{ queryNodeIndex("type:Instruction AND code:call*") }
	.scatter()
	.filter{ it.code.contains(String.format('<%s,', funcAddr )) }
})
