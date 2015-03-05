/**
   This module contains index lookup functions employed to provide
   start node sets for traversals. All of these lookups support wild
   cards (you will need to escape spaces though).
*/


/**
   Retrieve nodes from index using a Lucene query.

   @param query The lucene query to run

*/

Object.metaClass.queryNodeIndex = { query ->
	index = g.getRawGraph().index().forNodes(NODE_INDEX)
	try{
	    new Neo4j2VertexIterable(index.query(query), g)._()
	}catch(ParseException){
	  return []._()
	}
}

Object.metaClass.getFunctionsByName = { name ->
	query = String.format("type:%s AND code:%s", TYPE_FUNCTION_DEF, name)
	queryNodeIndex(query)
}
