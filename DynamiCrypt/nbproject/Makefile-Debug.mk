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
include DynamiCrypt-Makefile.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/8cd854a2/API_service.o \
	${OBJECTDIR}/_ext/8cd854a2/API_service_data_handler.o \
	${OBJECTDIR}/_ext/8cd854a2/SingleTpmNetworkHandler.o \
	${OBJECTDIR}/_ext/8cd854a2/System_Helper.o \
	${OBJECTDIR}/_ext/8cd854a2/TpmNetworkHandler.o \
	${OBJECTDIR}/_ext/8cd854a2/definitions.o \
	${OBJECTDIR}/_ext/8cd854a2/peer.o \
	${OBJECTDIR}/APIServer.o \
	${OBJECTDIR}/TPMInputVector.o \
	${OBJECTDIR}/TpmHandler.o \
	${OBJECTDIR}/TreeParityMachine.o \
	${OBJECTDIR}/main.o


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
LDLIBSOPTIONS=-lpthread -lboost_system -lboost_thread -lboost_program_options -lcryptopp -lpistache

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicrypt

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicrypt: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicrypt ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/8cd854a2/API_service.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/API_service.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8cd854a2
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8cd854a2/API_service.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/API_service.cpp

${OBJECTDIR}/_ext/8cd854a2/API_service_data_handler.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/API_service_data_handler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8cd854a2
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8cd854a2/API_service_data_handler.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/API_service_data_handler.cpp

${OBJECTDIR}/_ext/8cd854a2/SingleTpmNetworkHandler.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/SingleTpmNetworkHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8cd854a2
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8cd854a2/SingleTpmNetworkHandler.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/SingleTpmNetworkHandler.cpp

${OBJECTDIR}/_ext/8cd854a2/System_Helper.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/System_Helper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8cd854a2
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8cd854a2/System_Helper.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/System_Helper.cpp

${OBJECTDIR}/_ext/8cd854a2/TpmNetworkHandler.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/TpmNetworkHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8cd854a2
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8cd854a2/TpmNetworkHandler.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/TpmNetworkHandler.cpp

${OBJECTDIR}/_ext/8cd854a2/definitions.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/definitions.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8cd854a2
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8cd854a2/definitions.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/definitions.cpp

${OBJECTDIR}/_ext/8cd854a2/peer.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/peer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/8cd854a2
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8cd854a2/peer.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCrypt/peer.cpp

${OBJECTDIR}/APIServer.o: APIServer.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/APIServer.o APIServer.cpp

${OBJECTDIR}/TPMInputVector.o: TPMInputVector.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TPMInputVector.o TPMInputVector.cpp

${OBJECTDIR}/TpmHandler.o: TpmHandler.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TpmHandler.o TpmHandler.cpp

${OBJECTDIR}/TreeParityMachine.o: TreeParityMachine.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeParityMachine.o TreeParityMachine.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

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
