set VCTargetsPath=c:\Program Files (x86)\MSBuild\Microsoft.Cpp\v4.0\V120

msbuild ..\ArcaneCollate.sln /v:n /p:Configuration=collate_lib
msbuild ..\ArcaneCollate.sln /v:n /p:Configuration=cmd
msbuild ..\ArcaneCollate.sln /v:n /p:Configuration=tests
