##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Server
ConfigurationName      :=Debug
WorkspacePath          :=/home/mxg/Documents/SocketProgramming2/WS1718_VSYS
ProjectPath            :=/home/mxg/Documents/SocketProgramming2/WS1718_VSYS/Server
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
<<<<<<< HEAD
User                   :=Alex Schaller
Date                   :=07/11/17
CodeLitePath           :="/home/pinkaru/.codelite"
=======
User                   :=mxg
Date                   :=07/11/17
CodeLitePath           :=/home/mxg/.codelite
>>>>>>> 4046a0ecfd4a51128f3b89c5d9c4e4c6deff35a0
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Server.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
<<<<<<< HEAD
LinkOptions            := -lpthread /usr/lib/x86_64-linux-gnu/libldap.so /usr/lib/x86_64-linux-gnu/liblber.so -lldap 
=======
LinkOptions            := -lpthread /usr/lib/x86_64-linux-gnu/libldap.so  /usr/lib/x86_64-linux-gnu/liblber.so  -lldap 
>>>>>>> 4046a0ecfd4a51128f3b89c5d9c4e4c6deff35a0
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS := -DLDAP_DEPRECATED  -g -O0 -std=c++11 -Wall  $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/mxg/Documents/SocketProgramming2/WS1718_VSYS/Server/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(ObjectSuffix): src/Class/StreamSocketServer.cpp $(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/mxg/Documents/SocketProgramming2/WS1718_VSYS/Server/src/Class/StreamSocketServer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(DependSuffix): src/Class/StreamSocketServer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(DependSuffix) -MM src/Class/StreamSocketServer.cpp

$(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(PreprocessSuffix): src/Class/StreamSocketServer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Class_StreamSocketServer.cpp$(PreprocessSuffix) src/Class/StreamSocketServer.cpp

$(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(ObjectSuffix): src/Class/MessageHandlerServer.cpp $(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/mxg/Documents/SocketProgramming2/WS1718_VSYS/Server/src/Class/MessageHandlerServer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(DependSuffix): src/Class/MessageHandlerServer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(DependSuffix) -MM src/Class/MessageHandlerServer.cpp

$(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(PreprocessSuffix): src/Class/MessageHandlerServer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Class_MessageHandlerServer.cpp$(PreprocessSuffix) src/Class/MessageHandlerServer.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


