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
CND_PLATFORM=GNU-Linux
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
	${OBJECTDIR}/bookEcho/Echo1.o \
	${OBJECTDIR}/bookEcho/clientAsync.o \
	${OBJECTDIR}/bookEcho/echoServerSync.o \
	${OBJECTDIR}/bookEcho/seriousClientAndServer/async/client.o \
	${OBJECTDIR}/bookEcho/seriousClientAndServer/async/server.o \
	${OBJECTDIR}/bookEcho/seriousClientAndServer/clientSync.o \
	${OBJECTDIR}/bookEcho/seriousClientAndServer/serverSync.o \
	${OBJECTDIR}/bookEcho/seriousClientAndServer/test/client.o \
	${OBJECTDIR}/bookEcho/seriousClientAndServer/test/server.o \
	${OBJECTDIR}/bookEcho/serverAsync.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/netowkingContinued.o


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
LDLIBSOPTIONS=-L/usr/lib -L. -lpthread -lboost_thread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicryptsync

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicryptsync: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicryptsync ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/bookEcho/Echo1.o: bookEcho/Echo1.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/Echo1.o bookEcho/Echo1.cpp

${OBJECTDIR}/bookEcho/clientAsync.o: bookEcho/clientAsync.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/clientAsync.o bookEcho/clientAsync.cpp

${OBJECTDIR}/bookEcho/echoServerSync.o: bookEcho/echoServerSync.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/echoServerSync.o bookEcho/echoServerSync.cpp

${OBJECTDIR}/bookEcho/seriousClientAndServer/async/client.o: bookEcho/seriousClientAndServer/async/client.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho/seriousClientAndServer/async
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/seriousClientAndServer/async/client.o bookEcho/seriousClientAndServer/async/client.cpp

${OBJECTDIR}/bookEcho/seriousClientAndServer/async/server.o: bookEcho/seriousClientAndServer/async/server.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho/seriousClientAndServer/async
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/seriousClientAndServer/async/server.o bookEcho/seriousClientAndServer/async/server.cpp

${OBJECTDIR}/bookEcho/seriousClientAndServer/clientSync.o: bookEcho/seriousClientAndServer/clientSync.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho/seriousClientAndServer
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/seriousClientAndServer/clientSync.o bookEcho/seriousClientAndServer/clientSync.cpp

${OBJECTDIR}/bookEcho/seriousClientAndServer/serverSync.o: bookEcho/seriousClientAndServer/serverSync.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho/seriousClientAndServer
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/seriousClientAndServer/serverSync.o bookEcho/seriousClientAndServer/serverSync.cpp

${OBJECTDIR}/bookEcho/seriousClientAndServer/test/client.o: bookEcho/seriousClientAndServer/test/client.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho/seriousClientAndServer/test
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/seriousClientAndServer/test/client.o bookEcho/seriousClientAndServer/test/client.cpp

${OBJECTDIR}/bookEcho/seriousClientAndServer/test/server.o: bookEcho/seriousClientAndServer/test/server.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho/seriousClientAndServer/test
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/seriousClientAndServer/test/server.o bookEcho/seriousClientAndServer/test/server.cpp

${OBJECTDIR}/bookEcho/serverAsync.o: bookEcho/serverAsync.cpp
	${MKDIR} -p ${OBJECTDIR}/bookEcho
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bookEcho/serverAsync.o bookEcho/serverAsync.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/netowkingContinued.o: netowkingContinued.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/netowkingContinued.o netowkingContinued.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc