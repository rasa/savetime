all:
	VCBuild.exe /nologo savetime.vcproj /rebuild

clean:
	VCBuild.exe /nologo savetime.vcproj /clean
	
upgrade:
	devenv savetime.sln /upgrade

.PHONY:	all clean upgrade

