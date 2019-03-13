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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include DynamiCryptSync-Makefile.mk

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/9a0c25d/SingleTpmNetworkHandler.o \
	${OBJECTDIR}/_ext/9a0c25d/System_Helper.o \
	${OBJECTDIR}/_ext/9a0c25d/TpmNetworkHandler.o \
	${OBJECTDIR}/_ext/9a0c25d/definitions.o \
	${OBJECTDIR}/_ext/9a0c25d/peer.o \
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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicryptsync

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicryptsync: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynamicryptsync ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/9a0c25d/SingleTpmNetworkHandler.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/SingleTpmNetworkHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9a0c25d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9a0c25d/SingleTpmNetworkHandler.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/SingleTpmNetworkHandler.cpp

${OBJECTDIR}/_ext/9a0c25d/System_Helper.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/System_Helper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9a0c25d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9a0c25d/System_Helper.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/System_Helper.cpp

${OBJECTDIR}/_ext/9a0c25d/TpmNetworkHandler.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/TpmNetworkHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9a0c25d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9a0c25d/TpmNetworkHandler.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/TpmNetworkHandler.cpp

${OBJECTDIR}/_ext/9a0c25d/definitions.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/definitions.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9a0c25d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9a0c25d/definitions.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/definitions.cpp

${OBJECTDIR}/_ext/9a0c25d/peer.o: /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/peer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9a0c25d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9a0c25d/peer.o /media/veracrypt1/CollegeSyncUnison/FYP/code/Dynamicrypt/git/DynamiCrypt/DynamiCryptSync/peer.cpp

${OBJECTDIR}/TPMInputVector.o: TPMInputVector.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TPMInputVector.o TPMInputVector.cpp

${OBJECTDIR}/TpmHandler.o: TpmHandler.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TpmHandler.o TpmHandler.cpp

${OBJECTDIR}/TreeParityMachine.o: TreeParityMachine.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TreeParityMachine.o TreeParityMachine.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

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
