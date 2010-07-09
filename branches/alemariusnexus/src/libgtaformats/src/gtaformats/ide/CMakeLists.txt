SET(FILES IDEAnimation IDEEntity IDEReader IDEStatement IDEStaticObject IDETimedObject)

FOREACH(FILE ${FILES})
    SET(SRCS ${SRCS} "gtaformats/ide/${FILE}")
ENDFOREACH(FILE)

SET(SRCS ${SRCS} PARENT_SCOPE)
