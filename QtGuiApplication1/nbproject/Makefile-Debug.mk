#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/GuiGenerator.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Facet.o \
	${OBJECTDIR}/MTXPoint.o \
	${OBJECTDIR}/Marker.o \
	${OBJECTDIR}/Vector.o \
	${OBJECTDIR}/UtilityFunctions.o \
	${OBJECTDIR}/Collection.o \
	${OBJECTDIR}/XPoints.o \
	${OBJECTDIR}/Persistence.o \
	${OBJECTDIR}/Xform3D.o \
	${OBJECTDIR}/Markers.o \
	${OBJECTDIR}/MCamera.o \
	${OBJECTDIR}/Cameras.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../Dist -Llib -lMTC -ldc1394 -lraw1394 -lpthread -lvnl -lvnl_algo -lvcl -lv3p_netlib -lfltk -lfltk_forms -lfltk_gl -lfltk_images -lGL -lXft

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mtdemocpp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mtdemocpp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mtdemocpp ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/GuiGenerator.o: GuiGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GuiGenerator.o GuiGenerator.cpp

${OBJECTDIR}/main.o: main.cc 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cc

${OBJECTDIR}/Facet.o: Facet.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet.o Facet.cpp

${OBJECTDIR}/MTXPoint.o: MTXPoint.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/MTXPoint.o MTXPoint.cpp

${OBJECTDIR}/Marker.o: Marker.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/Marker.o Marker.cpp

${OBJECTDIR}/Vector.o: Vector.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/Vector.o Vector.cpp

${OBJECTDIR}/UtilityFunctions.o: UtilityFunctions.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/UtilityFunctions.o UtilityFunctions.cpp

${OBJECTDIR}/Collection.o: Collection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/Collection.o Collection.cpp

${OBJECTDIR}/XPoints.o: XPoints.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/XPoints.o XPoints.cpp

${OBJECTDIR}/Persistence.o: Persistence.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/Persistence.o Persistence.cpp

${OBJECTDIR}/Xform3D.o: Xform3D.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/Xform3D.o Xform3D.cpp

${OBJECTDIR}/Markers.o: Markers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/Markers.o Markers.cpp

${OBJECTDIR}/MCamera.o: MCamera.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/MCamera.o MCamera.cpp

${OBJECTDIR}/Cameras.o: Cameras.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Dist -IFL -MMD -MP -MF $@.d -o ${OBJECTDIR}/Cameras.o Cameras.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mtdemocpp

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
