all:
	MSBuild.exe /nologo savetime.sln /p:Configuration=Debug
	MSBuild.exe /nologo savetime.sln /p:Configuration=Release

clean:
	MSBuild.exe /nologo savetime.sln /p:Configuration=Debug   /t:clean
	MSBuild.exe /nologo savetime.sln /p:Configuration=Release /t:clean

upgrade:
	devenv savetime.sln /upgrade
	
.PHONY:	all clean upgrade


