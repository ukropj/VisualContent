PROJECT(Diplomovka)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# Where to install module parts:
SET (INSTALL_BIN . CACHE PATH "Directory for binaries.")
SET (INSTALL_LIB . CACHE PATH "Directory for external libraries.")
SET (INSTALL_INC include CACHE PATH "Directory for library headers.")	
SET (INSTALL_DATA . CACHE PATH "Directory the package can store documentation, tests or other data in.")
SET (INSTALL_DOC ${INSTALL_DATA}/doc CACHE PATH "Recommended directory to install documentation into.")
SET (INSTALL_ETC ${INSTALL_DATA}/etc CACHE PATH "Other files.")
SET (INSTALL_TEST ${INSTALL_DATA}/test CACHE PATH "Test files.")
SET (INSTALL_EXAMPLE ${INSTALL_DATA}/example CACHE PATH "Recommended directory to install examples into.")

### Find libraries ###

# Where to search
IF (WIN32)
	SET (CMAKE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Windows/include
				${CMAKE_INCLUDE_PATH})
	SET (CMAKE_LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Windows/lib
				${CMAKE_LIBRARY_PATH})
	MESSAGE("LIB PATH:"${CMAKE_LIBRARY_PATH})
ELSEIF(UNIX)
	SET (CMAKE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Linux/include
				${CMAKE_INCLUDE_PATH})

	SET (CMAKE_LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Linux/lib
				${CMAKE_LIBRARY_PATH})
ENDIF()


SET (CMAKE_INCLUDE_PATH 
#~	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Windows/include
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Source/libnoisesrc-1.0.0/src
	${CMAKE_INCLUDE_PATH})


#~ FIND_PACKAGE(ZLIB NO_DEFAULT_PATH)
#~ FIND_PACKAGE(PNG NO_DEFAULT_PATH)

FIND_PACKAGE(ZLIB)
FIND_PACKAGE(PNG )


# Find Qt
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL 1)
SET(QT_USE_QTSQL 1)
SET(QT_USE_QTWEBKIT 1)
SET(QT_USE_QTXML 1)
SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} )

# Find OpenSceneGraph
FIND_PACKAGE(OpenSceneGraph COMPONENTS 
osgGA 
osgUtil 
OpenThreads 
osg 
osgAnimation 
osgDB
osgFX
osgManipulator
osgParticle
osgShadow
osgSim
osgTerrain
osgText
osgVolume
osgWidget
osgViewer)

MESSAGE ("osg libraries:: ${OPENSCENEGRAPH_LIBRARIES}")

# Include headers
INCLUDE_DIRECTORIES(
${PNG_INCLUDE_DIRECTORIES}
${ZLIB_INCLUDE_DIRECTORIES}
${OPENSCENEGRAPH_INCLUDE_DIRS} 
${OSGVIEWER_INCLUDE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_INCLUDE_PATH}
${CMAKE_CURRENT_BINARY_DIR} "./")

# Build dependencies
ADD_SUBDIRECTORY(dependencies/Source)

# Source files
FILE(GLOB_RECURSE SRC  "src/*.cpp")
FILE(GLOB_RECURSE INCL "include/*.h")

# .h subory, ktore obsahuju Q_OBJECT
SET(SOURCES_H 
	./include/OsgQtBrowser/QGraphicsViewAdapter.h 
	./include/Window/CoreWindow.h
	./include/Window/OptionsWindow.h
	./include/Window/ProgressManager.h
	./include/Window/CheckBoxList.h
	./include/Window/DataMappingDialog.h	
	./include/Viewer/PickHandler.h
	./include/Viewer/AbstractNode.h
	./include/Viewer/OsgNodeGroup.h
	./include/Viewer/ControlFrame.h
)

# toto makro spracuje Q_OBJECT a vygeneruje novy cpp subor, ktory bude dostupny v ${SOURCES_H_MOC} premennej
QT4_WRAP_CPP( SOURCES_H_MOC ${SOURCES_H} )

ADD_EXECUTABLE(VisualContent ${INCL} ${SOURCES_H_MOC} ${SRC})

SOURCE_GROUP(\\src\\Viewer "^.*Viewer/.*$")
SOURCE_GROUP(\\src\\Core "^.*Core/.*$")
SOURCE_GROUP(\\src\\Model "^.*Model/.*$")
SOURCE_GROUP(\\src\\Util "^.*Util/.*$")
SOURCE_GROUP(\\src\\Window "^.*Window/.*$")
SOURCE_GROUP(\\src\\OsgQtBrowser "^.*OsgQtBrowser/.*$")
SOURCE_GROUP(\\src\\Noise "^.*Noise/.*$")

SOURCE_GROUP(\\headers\\Viewer "^.*Viewer/.*h$")
SOURCE_GROUP(\\headers\\Core "^.*Core/.*h$")
SOURCE_GROUP(\\headers\\Model "^.*Model/.*h$")
SOURCE_GROUP(\\headers\\Util "^.*Util/.*h$")
SOURCE_GROUP(\\headers\\Window "^.*Window/.*h$")
SOURCE_GROUP(\\headers\\Noise "^.*Noise/.*h$")
SOURCE_GROUP(\\headers\\OsgQtBrowser "^.*OsgQtBrowser/.*h$")

SOURCE_GROUP(\\MOC "^.*moc_.*$")

TARGET_LINK_LIBRARIES(VisualContent
${QT_LIBRARIES}  
${OPENSCENEGRAPH_LIBRARIES} 
${OSGVIEWER_LIBRARIES}
${PNG_LIBRARIES}
${ZLIB_LIBRARIES}
${ZLIB_LIBRARY}
${PNG_LIBRARY}
noise
)

#~ INSTALL(TARGETS 3DVisual DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/_INSTALL/Debug CONFIGURATIONS Debug) 
#~ INSTALL(TARGETS 3DVisual DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/_install/Release CONFIGURATIONS Release) 
#~ INSTALL(TARGETS 3DVisual RUNTIME DESTINATION ${INSTALL_BIN}) 


#~ INSTALL(DIRECTORY img DESTINATION ${INSTALL_DATA} PATTERN ".svn" EXCLUDE)
#~ INSTALL(DIRECTORY img DESTINATION ${CMAKE_CURRENT_BINARY_DIR} PATTERN ".svn" EXCLUDE)
