@ECHO OFF

cd "C:\Users\TheKronnY\Downloads\VUT-FIT-IFJ-2017-toolkit-master1-3-9\VUT-FIT-IFJ-2017-toolkit-master"

ifjcode17-tests --no-colors --command-timeout 10 -e "G:\GoogleDrive\PRIVATE\_VUT\2ZS\IFJ\IFJ_Project\src\rozsireni" "G:\GoogleDrive\PRIVATE\_VUT\2ZS\IFJ\IFJ_Project\build\bin\Debug\IFJ2017_Project.exe" 2>test_debug.log

set /p x=Press enter to continue . . . | 